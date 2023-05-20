#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <iostream>
#include <memory>

#include "poll_server.hpp"
#include "http_response.hpp"

PollServer::PollServer(const std::string &port, std::shared_ptr<Router> router) : ServerRunner(), port_(port)
{
    router_ = router;

    // Get us a socket and bind it
    struct addrinfo hints, *ai, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &ai)) != 0)
    {
        fprintf(stderr, "PollServer: %s\n", gai_strerror(rv));
        exit(1);
    }

    // Break after the socket is created and bound
    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener_ < 0)
        {
            continue;
        }

        // Lose "address already in use" error
        int yes = 1;
        setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener_, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener_);
            continue;
        }

        break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL)
    {
        std::cerr << "PollServer: failed to bind" << std::endl;
        std::cerr << "Reason: " << std::strerror(errno) << std::endl;
        exit(1);
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener_, 128) == -1)
    {
        fprintf(stderr, "PollServer: listen error\n");
        exit(1);
    }

    accepter_thread_ref_ = std::thread(&PollServer::accepter_thread, this);
}

void PollServer::accepter_thread()
{
    while (!stop_)
    {
        accept_new_connection();
    }
}

PollServer::PollServer(const std::string &port, std::shared_ptr<Router> router, int max_thread_counter) : PollServer(port, router)
{
    max_threads_ = max_thread_counter;
}

PollServer::~PollServer()
{
    stop_ = true;
    if (accepter_thread_ref_.joinable())
        accepter_thread_ref_.join();

    pfds_.clear();
}

void PollServer::run()
{
    auto pool = std::make_unique<thread_pool>(max_threads_);
    // Remove this while loop altogether. Use 'select' system call inside
    // 'handle_client' function to wait until the socket becomes readable.
    // Call 'handle_client' from 'accept_new_connection'
    while (!stop_)
    {
        std::unique_lock<std::shared_mutex> lock(pfds_lock_);

        int size = pfds_.size();

        std::unique_ptr<pollfd[]> pfds_copy(new pollfd[size + 1]);
        std::copy(pfds_.begin(), pfds_.end(), pfds_copy.get());

        pfds_.clear();

        lock.unlock();

        pollfd pfd;
        pfd.fd = listener_;
        pfd.events = POLLIN;
        pfds_copy[size] = pfd;

        int poll_count = poll(pfds_copy.get(), size + 1, 500);
        if (poll_count == -1)
        {
            perror("PollServer: poll error");
            exit(1);
        }

        int i = 0;
        while (i < size)
        {
            auto top = pfds_copy[i];
            // Check if someone's ready to read
            if (top.revents & POLLIN)
            {
                // If not the listener, we're just a regular client...
                pool->enqueue(&PollServer::handle_client, this, top.fd);
            } else {
                lock.lock();
                pfds_.push_front(top);
                lock.unlock();
            }
            i++;
        }
    }
}

void PollServer::accept_new_connection()
{
    int newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];
    
    // If listener is ready to read, handle new connection
    addrlen = sizeof remoteaddr;
    newfd = accept(listener_,
                   (struct sockaddr *)&remoteaddr,
                   &addrlen);

    if (newfd == -1)
    {
        perror("accept");
    }
    else
    {
        add_to_pfds(newfd);

        printf("pollserver: new connection from %s on "
               "socket %d\n",
               inet_ntop(remoteaddr.ss_family,
                         get_in_addr((struct sockaddr *)&remoteaddr),
                         remoteIP, INET6_ADDRSTRLEN),
               newfd);
    }
}

void PollServer::handle_client(int fd)
{
    int client_socket = fd;
    std::string request_str = receive_http_request(client_socket);

    // Connection closed
    if (request_str.size() == 0)
    {
        close(fd);
        return;
    };

    parser_.parse(request_str.c_str(), request_str.size());

    HttpRequest request = parser_.getRequest();
    std::unique_ptr<Sender> sender = std::make_unique<Sender>(client_socket);

    // @todo: handle exceptions
    auto path = request.getPath();

    auto handler = router_->findRoute(path.c_str());

    handler->injectSender(sender);
    handler->Handle(request);

    close(fd);
}

void PollServer::add_to_pfds(int newfd)
{
    pollfd new_pfd;

    new_pfd.fd = newfd;
    new_pfd.events = POLLIN;

    std::unique_lock<std::shared_mutex> lock(pfds_lock_);
    pfds_.push_front(new_pfd);
}

// Get sockaddr, IPv4 or IPv6:
void *PollServer::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


// Wrapper function for recv() that receives an HTTP request
// message from a socket and returns the message as a string.
std::string PollServer::receive_http_request(int sockfd)
{
    // Initialize a buffer to receive the data.
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    
    // Initialize a string to hold the received data.
    std::string request_message;

    // Receive data from the socket until the entire message has been received.
    while (true)
    {
        // Receive data from the socket and check for errors.
        int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0)
        {
            return "";
        }
        else if (bytes_received == 0)
        {
            return "";
        }

        // Append the received data to the message.
        request_message.append(buffer, bytes_received);

        // Check if we have received the entire message.
        std::string::size_type pos = request_message.find("\r\n\r\n");

        if (pos != std::string::npos)
        {
            // Found the end of the headers.
            // Check if the message includes a message body.
            std::string content_length_header = "Content-Length:";
            std::string::size_type content_length_pos = request_message.find(content_length_header);
            if (content_length_pos != std::string::npos)
            {
                // The message includes a message body.
                // Find the end of the headers.
                std::string::size_type end_of_headers_pos = pos + 4;

                // Find the start and end of the message body.
                std::string::size_type start_of_body_pos = end_of_headers_pos;
                std::string::size_type end_of_body_pos = start_of_body_pos + std::stoi(request_message.substr(content_length_pos + content_length_header.length()));
                if (end_of_body_pos <= request_message.length())
                {
                    // The entire message has been received.
                    return request_message;
                }
            }
            else
            {
                // The message does not include a message body.
                // The entire message has been received.
                return request_message;
            }
        }
    }

    // Control should never reach here.
    return "";
}
