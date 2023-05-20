/**
 * High level process
 * We create a bunch of routes
 * We assign a bunch of handlers to those routes
 * We start our server -> it listens for requests on a port
 * It receives the request -> we sanitize it, check the validity
 * If it is valid and has the necessary data, we try to look for the handler in the router
 * If it is present, we call the method
 * If at some point we cannot find something, we send the corresponding error
 *
 * Notes:
 * Decide on graceful error handling
 * Think about the ways to pass around the info about the request(request object: type, path, query params, body params, ip of a sender)
 * Use thread pool in the threads example
 *
 * Down the road:
 * Think about a way to enable middleware
 * Think of a logger
 * Think of a https handling
 * think about an authentication handler
 *
 * @todo Create a diagram with all the main classes: Server, Router, Route, RouteHandler, Request, Response
 *
 */

#ifndef SERVER_H
#define SERVER_H

#include "router.hpp"
#include "server_runner.hpp"

struct Config
{
    std::shared_ptr<Router> router;
    ServerRunnerTypes::ServerRunnerType server_runner_type;
    int port;
    int threadCounter = 8;
};

class Server
{
private:
    int port_;
    std::shared_ptr<Router> router_;
    std::unique_ptr<ServerRunner> server_runner_;

public:
    Server(Config config);
    ~Server();

    void start();
    void stop();
};
#endif