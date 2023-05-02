#include "http_response.hpp"
#include <sys/socket.h>

class Sender {
public:
    Sender(int new_socket_fd) {
        socket_fd = new_socket_fd;
    }
    void sendResponse(HttpResponse& response) {
        auto response_string = response.toString();
        // @todo: handle errors from send_all
        int bytes_sent = send_all(socket_fd, response_string.c_str(), response_string.size(), 0);
    }
private:
    ssize_t send_all(int sockfd, const void *buf, size_t len, int flags) {
        size_t total_sent = 0;
        const char *pbuf = static_cast<const char*>(buf);

        while (total_sent < len) {
            ssize_t bytes_sent = send(sockfd, pbuf + total_sent, len - total_sent, flags);

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