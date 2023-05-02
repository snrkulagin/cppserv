#ifndef ROUTER_H
#define ROUTER_H

#include <unordered_map>
#include "route_handler.hpp"

class DefaultRoute : public BaseRouteHandler {
    public:
    void Handle(HttpRequest& req) override {
        sendNotFound();
    }
};

class Router {
    private:
    std::unordered_map<RoutePath, BaseRouteHandler*> routes;

    public:

    void addRoute(RoutePath path, BaseRouteHandler* handler);
    BaseRouteHandler* findRoute(RoutePath path);
};

#endif