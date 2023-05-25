#include "http_response.hpp"
#include <sys/socket.h>
#include <iostream>
#include <sys/select.h>


class Sender {
public:
    Sender(int new_socket_fd) {
        setFd(new_socket_fd);
    }

    // This should be in a standalone class
    // like static SocketHelper class
    bool wait_till_fd_ready_for_writing(int fd) {
        fd_set writeSet;
        FD_ZERO(&writeSet);
        FD_SET(fd, &writeSet);

        int ready = select(fd + 1, nullptr, &writeSet, nullptr, nullptr);
        if (ready == -1) {
            std::cout << "error: Couldn't write to socket " << fd << std::endl;
            perror("error");
            return false;
        } else {
            if (FD_ISSET(fd, &writeSet)) {
                return true;
            }

            return false;
        }
    }
    void sendResponse(HttpResponse& response) {
        if (socket_fd == 0) return;

        bool isOk = wait_till_fd_ready_for_writing(socket_fd);

        if (!isOk) {
            std::cout << "select returned not okay" << std::endl;
            return;
        }

        auto response_string = response.toString();
        // @todo: handle errors from send_all
        int bytes_sent = send_all(socket_fd, response_string.c_str(), response_string.size(), 0);

        if (bytes_sent == -1) {
            std::cout << "error: was not able to send the whole response" << std::endl;
            perror("error");
            std::cout << "sockfd: " << socket_fd << std::endl;
        }
    }

    void setFd(int fd) {
        socket_fd = fd;
    }

private:
    int send_all(int sockfd, const void *buf, size_t len, int flags) {
        int total_sent = 0;
        const char *pbuf = static_cast<const char*>(buf);
        
        while (total_sent < len) {
            int bytes_sent = send(sockfd, pbuf + total_sent, len - total_sent, flags);

            if (bytes_sent < 0) {
                return bytes_sent; // Error occurred
            } else if (bytes_sent == 0) {
                return total_sent; // Connection closed
            } else {
                total_sent += bytes_sent;
            }
        }

        return total_sent;
    }

    int socket_fd = -1;
};