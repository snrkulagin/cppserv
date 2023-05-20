#ifndef POLL_SERVER_H
#define POLL_SERVER_H

#include <string>
#include <poll.h>
#include <vector>
#include <shared_mutex>
#include "http_request.hpp"
#include "server_runner.hpp"
#include "router.hpp"
#include "thread_pool.hpp"

class PollServer : public ServerRunner
{
public:
    PollServer(const std::string &port, std::shared_ptr<Router> router);
    PollServer(const std::string &port, std::shared_ptr<Router> router, int max_thread_counter);
    ~PollServer();

    void run();

private:
    void *get_in_addr(struct sockaddr *sa);
    void add_to_pfds(int newfd);
    void del_from_pfds_by_fd(int fd);

    void handle_client(int i);
    void accept_new_connection();
    void accepter_thread();

    std::string receive_http_request(int sockfd);

    static const int INITIAL_PFD_SIZE = 5;

    std::shared_ptr<Router> router_;

    HttpRequestParser parser_;
    const std::string port_;
    int listener_;
    //std::vector<pollfd> pfds_;
    std::deque<pollfd> pfds_;

    std::thread accepter_thread_ref_;

    int max_threads_ = -1;
    std::shared_mutex pfds_lock_;
};


#endif // POLL_SERVER_H
