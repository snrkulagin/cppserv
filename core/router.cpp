#include "router.hpp"

// Default route for 404
DefaultRoute def;

void Router::addRoute(RoutePath path, BaseRouteHandler* handler) {
    routes.insert({ path, handler });
}

BaseRouteHandler* Router::findRoute(RoutePath path) {
    auto route = routes.find(path);

    if (route == routes.end()) {
        return &def;
    }

    return (*route).second;
}
