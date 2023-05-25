#ifndef SELECT_SERVER_H
#define SELECT_SERVER_H

#include <string>
#include <poll.h>
#include <vector>
#include <shared_mutex>
#include "http_request.hpp"
#include "server_runner.hpp"
#include "router.hpp"
#include "thread_pool.hpp"

class SelectServer : public ServerRunner
{
public:
    SelectServer(const std::string &port, std::shared_ptr<Router> router);
    SelectServer(const std::string &port, std::shared_ptr<Router> router, int max_thread_counter);
    ~SelectServer();

    void run();

private:
    void *get_in_addr(struct sockaddr *sa);

    void handle_client(int i);
    void accept_new_connection();
    void accepter_thread();

    bool wait_till_fd_ready(int fd);

    std::string receive_http_request(int sockfd);

    std::shared_ptr<Router> router_;

    HttpRequestParser parser_;
    
    const std::string port_;
    int listener_;

    std::thread accepter_thread_ref_;

    int max_threads_ = -1;

    std::unique_ptr<thread_pool> t_pool;
};


#endif // SELECT_SERVER_H
