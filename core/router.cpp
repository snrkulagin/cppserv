#include "router.hpp"

void Router::addRoute(RoutePath& path, BaseRouteHandler& handler) {
    routes.insert({ path, handler });
}

DefaultRoute def;

BaseRouteHandler& Router::findRoute(RoutePath& path) {
    auto route = routes.find(path);

    if (route == routes.end()) {
        throw no_route_found(path + " was not found");
    }

    return (*route).second;
}

int Router::findRoute(RoutePath& path, BaseRouteHandler& handler) {
    auto route = routes.find(path);

    if (route == routes.end()) {
        handler = def;
        return 0;
    }

    handler = (*route).second;

    return 0;
}