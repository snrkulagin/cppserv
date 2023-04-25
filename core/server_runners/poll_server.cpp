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

#include "poll_server.hpp"

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
        fprintf(stderr, "PollServer: failed to bind\n");
        exit(1);
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener_, 10) == -1)
    {
        fprintf(stderr, "PollServer: listen error\n");
        exit(1);
    }

    // Add the listener to set
    pfds_ = new pollfd[INITIAL_PFD_SIZE];
    pfds_[0].fd = listener_;
    pfds_[0].events = POLLIN;
    fd_count_ = 1;
    fd_size_ = INITIAL_PFD_SIZE;
}

PollServer::~PollServer()
{
    delete[] pfds_;
}

void PollServer::run()
{
    while (!stop_)
    {
        int poll_count = poll(pfds_, fd_count_, -1);

        if (poll_count == -1)
        {
            perror("PollServer: poll error");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for (int i = 0; i < fd_count_; i++)
        {
            // Check if someone's ready to read
            if (pfds_[i].revents & POLLIN)
            {
                if (pfds_[i].fd == listener_)
                {
                    // If listener is ready to read, handle new connection
                    accept_new_connection();
                }
                else
                {
                    // If not the listener, we're just a regular client...
                    handle_client(i);
                }
            }
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

void PollServer::handle_client(int i)
{
    std::string request_str = receive_http_request(pfds_[i].fd);
    parser_.parse(request_str.c_str(), request_str.size());

    HttpRequest request = parser_.getRequest();
}

void PollServer::add_to_pfds(int newfd)
{
    // If we don't have room, add more space in the pfds array
    pollfd *new_pfds = (pollfd *)realloc(pfds_, sizeof(*pfds_) * fd_size_);
    if (new_pfds == NULL)
    {
        free(pfds_); // free memory allocated for pfds_ before it was nulled
        pfds_ = NULL;
        return;
    }
    pfds_ = new_pfds;

    pfds_[fd_count_].fd = newfd;
    pfds_[fd_count_].events = POLLIN; // Check ready-to-read

    (fd_count_)++;
}

void PollServer::del_from_pfds(int i)
{
    // Copy the one from the end over this one
    pfds_[i] = pfds_[fd_count_ - 1];

    fd_count_--;
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
            // Handle error.
            return "";
        }
        else if (bytes_received == 0)
        {
            // Connection closed by peer.
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
