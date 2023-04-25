#include "server.hpp"
#include "server_runners/poll_server.hpp"

Server::Server(Config config)
{
    port_ = config.port;

    if (config.server_runner_type == ServerRunnerTypes::POLL)
    {
        server_runner_ = std::make_unique<PollServer>(std::to_string(port_), config.router);
    }
}

void Server::start()
{
    server_runner_->run();
}

void Server::stop() {
    server_runner_->stop();
}