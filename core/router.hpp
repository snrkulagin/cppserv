#ifndef ROUTER_H
#define ROUTER_H

#include <unordered_map>
#include "route_handler.hpp"

class no_route_found : public std::exception {
public:
    no_route_found(const std::string& message) : message_(message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;
};

class DefaultRoute : public BaseRouteHandler {
    public:
    void Handle(HttpRequest& req) override {
        sendNotFound();
    }
};

class Router {
    private:

    std::unordered_map<RoutePath, BaseRouteHandler> routes;

    public:

    void addRoute(RoutePath& path, BaseRouteHandler& handler);
    // If not found, send 404
    BaseRouteHandler& findRoute(RoutePath& path);
    int findRoute(RoutePath& path, BaseRouteHandler& handler);
};

#endif