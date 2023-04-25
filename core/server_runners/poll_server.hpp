#ifndef POLL_SERVER_H
#define POLL_SERVER_H

#include <string>
#include <poll.h>
#include "http_request.hpp"
#include "server_runner.hpp"
#include "router.hpp"

class PollServer : public ServerRunner {
public:
    PollServer(const std::string &port, std::shared_ptr<Router> router);
    ~PollServer();

    void run();

private:
    void *get_in_addr(struct sockaddr *sa);
    void add_to_pfds(int newfd);
    void del_from_pfds(int i);

    void handle_client(int i);
    void accept_new_connection();

    std::string receive_http_request(int sockfd);

    static const int INITIAL_PFD_SIZE = 5;

    std::shared_ptr<Router> router_;
    
    HttpRequestParser parser_;
    const std::string port_;
    int listener_;
    pollfd *pfds_;
    int fd_count_;
    int fd_size_;
};

#endif  // POLL_SERVER_H
