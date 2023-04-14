#ifndef POLL_SERVER_H
#define POLL_SERVER_H

#include <string>
#include <poll.h>

class PollServer {
public:
    PollServer(const std::string& port);

    void run();

private:
    void *get_in_addr(struct sockaddr *sa);
    int get_listener_socket();
    void add_to_pfds(pollfd **pfds, int newfd, int *fd_count, int *fd_size);
    void del_from_pfds(pollfd pfds[], int i, int *fd_count);

    void handle_client(int i);
    void accept_new_connection();

    std::string m_port;
    int m_listener;
    pollfd *m_pfds;
    int m_fd_count;
    int m_fd_size;
};

#endif  // POLL_SERVER_H
