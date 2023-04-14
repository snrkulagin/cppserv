#include "server.hpp"

Server::Server(Config config) {
    this->port = config.port;
    this->router = config.router;
}

void Server::start() {
    
}