#ifndef SERVER_RUNNER_H
#define SERVER_RUNNER_H

#include "request_parser.hpp"

namespace ServerRunnerTypes {
    enum ServerRunnerType {
        POLL,
        SELECT
    };
}
class ServerRunner {
    protected:
    int port_;
    std::atomic_bool stop_;

    public:
    ServerRunner() {
        stop_.store(false);
    }

    virtual ~ServerRunner() = default;

    void virtual run() = 0;
    void virtual stop() {
        stop_.store(true);
    }
};

#endif