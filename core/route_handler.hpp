#ifndef ROUTE_HANDLER_H
#define ROUTE_HANDLER_H

#include <string>

using RoutePath = std::string;

enum ReturnCodes {
    NOT_FOUND = 404,
    SUCCESS = 200
};

class BaseRouteHandler {
    private:

    RoutePath path;

    void send(int returnCode, std::string message);

    void sendNotFound() {
        send(NOT_FOUND, "Route was not found");
    }

    public:

    virtual ~BaseRouteHandler() {}

    virtual void Get() {
        sendNotFound();
    };
    virtual void Post() {
        sendNotFound();
    };
    virtual void Put() {
        sendNotFound();
    };
    virtual void Patch() {
        sendNotFound();
    };
    virtual void Delete() {
        sendNotFound();
    };
};
#endif