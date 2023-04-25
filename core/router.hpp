#ifndef ROUTER_H
#define ROUTER_H

#include <unordered_map>
#include "route_handler.hpp"

class Router {
    private:

    std::unordered_map<RoutePath, BaseRouteHandler> routes;

    public:

    void addRoute(RoutePath path, BaseRouteHandler handler);
    // If not found, send 404
    BaseRouteHandler& findRoute(RoutePath path);
};

#endif