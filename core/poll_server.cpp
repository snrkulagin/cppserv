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

class PollServer {
public:
    PollServer(const std::string& port) : port_(port) {
        // Get us a socket and bind it
        struct addrinfo hints, *ai, *p;
        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &ai)) != 0) {
            fprintf(stderr, "PollServer: %s\n", gai_strerror(rv));
            exit(1);
        }
        
        // Break after the socket is created and bound
        for (p = ai; p != NULL; p = p->ai_next) {
            listener_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (listener_ < 0) { 
                continue;
            }

            // Lose "address already in use" error
            int yes = 1;
            setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

            if (bind(listener_, p->ai_addr, p->ai_addrlen) < 0) {
                close(listener_);
                continue;
            }

            break;
        }

        // If we got here, it means we didn't get bound
        if (p == NULL) {
            fprintf(stderr, "PollServer: failed to bind\n");
            exit(1);
        }

        freeaddrinfo(ai); // All done with this

        // Listen
        if (listen(listener_, 10) == -1) {
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

    ~PollServer() {
        delete[] pfds_;
    }

    void run() {
        for (;;) {
            int poll_count = poll(pfds_, fd_count_, -1);

            if (poll_count == -1) {
                perror("PollServer: poll error");
                exit(1);
            }

            // Run through the existing connections looking for data to read
            for (int i = 0; i < fd_count_; i++) {
                // Check if someone's ready to read
                if (pfds_[i].revents & POLLIN) {
                    if (pfds_[i].fd == listener_) {
                        // If listener is ready to read, handle new connection
                        accept_new_connection();
                    } else {
                        // If not the listener, we're just a regular client...
                        handle_client(i);
                    }
                }
            }
        }
    }

private:
    static const int INITIAL_PFD_SIZE = 5;
    const std::string port_;
    int listener_;
    pollfd *pfds_;
    int fd_count_;
    int fd_size_;

    void accept_new_connection() {
        int newfd;
        struct sockaddr_storage remoteaddr;
        socklen_t addrlen;
        char remoteIP[INET6_ADDRSTRLEN];

        // If listener is ready to read, handle new connection
        addrlen = sizeof remoteaddr;
        newfd = accept(listener_,
            (struct sockaddr *)&remoteaddr,
            &addrlen);

        if (newfd == -1) {
            perror("accept");
        } else {
            add_to_pfds(newfd);

            printf("pollserver: new connection from %s on "
                "socket %d\n",
                inet_ntop(remoteaddr.ss_family,
                    get_in_addr((struct sockaddr*)&remoteaddr),
                    remoteIP, INET6_ADDRSTRLEN),
                newfd);
        }
    }

    void handle_client(int i) {
        // If not the listener, we're just a regular client
        /* 
            HERE IS WHERE I SHOULD HANDLE THE INCOMING HTTP REQUEST
            DON'T FORGET THAT RECV MIGHT NOT GIVE YOU THE FULL THING
        */
        char buf[1024];    // Buffer for client data

        int nbytes = recv(pfds_[i].fd, buf, sizeof buf, 0);

        printf("Hello World%s", buf);

        int sender_fd = pfds_[i].fd;

        if (nbytes <= 0) {
            // Got error or connection closed by client
            if (nbytes == 0) {
                // Connection closed
                printf("pollserver: socket %d hung up\n", sender_fd);
            } else {
                printf("omg");
                perror("recv");
            }

            close(pfds_[i].fd);

            del_from_pfds(i);

        } else {
            std::string err = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
            
            if (send(sender_fd, err.c_str(), err.size() * sizeof(char), 0) == -1) {
                perror("send");
            }
        }
    }

    void add_to_pfds(int newfd)
    {
        // If we don't have room, add more space in the pfds array
        pollfd *new_pfds = (pollfd *)realloc(pfds_, sizeof(*pfds_) * fd_size_);
        if (new_pfds == NULL) {
            free(pfds_); // free memory allocated for pfds_ before it was nulled
            pfds_ = NULL;
            return;
        }
        pfds_ = new_pfds;

        pfds_[fd_count_].fd = newfd;
        pfds_[fd_count_].events = POLLIN; // Check ready-to-read

        (fd_count_)++;
    }

    void del_from_pfds(int i)
    {
        // Copy the one from the end over this one
        pfds_[i] = pfds_[fd_count_ - 1];

        fd_count_--;
    }

    // Get sockaddr, IPv4 or IPv6:
    void* get_in_addr(struct sockaddr *sa)
    {
        if (sa->sa_family == AF_INET) {
            return &(((struct sockaddr_in*)sa)->sin_addr);
        }

        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
};
