#include "server.hpp"
#include "server_runners/poll_server.hpp"
#include "server_runners/select_server.hpp"

Server::Server(Config config)
{
    port_ = config.port;

    if (config.server_runner_type == ServerRunnerTypes::POLL)
    {
      server_runner_ = std::make_unique<PollServer>(std::to_string(port_), config.router, 8);
    } else if (config.server_runner_type == ServerRunnerTypes::SELECT) {
        server_runner_ = std::make_unique<SelectServer>(std::to_string(port_), config.router, 8);
    }
}

void Server::start()
{
    server_runner_->run();
}

void Server::stop() {
    server_runner_->stop();
}

Server::~Server() {
    stop();
}