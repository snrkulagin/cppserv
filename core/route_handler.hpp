#ifndef ROUTE_HANDLER_H
#define ROUTE_HANDLER_H

#include <string>
#include "sender.hpp"
#include "http_request.hpp"

using RoutePath = std::string;

enum ReturnCodes {
    NOT_FOUND = 404,
    SUCCESS = 200
};

class BaseRouteHandler {
    private:

    RoutePath path;

    protected:

    void sendNotFound() {
        HttpResponse resp;
        resp.setStatusCode(404);
        resp.setTextData("No such route found");
        sendResponse(resp);
    }

    public:

    std::function<void(HttpResponse&)> sendResponse;

    void injectSender(std::unique_ptr<Sender>& sender) {
        sendResponse = std::bind(&Sender::sendResponse, sender.get(), std::placeholders::_1);;
    };

    virtual ~BaseRouteHandler() {}

    virtual void Handle(HttpRequest& req) {
        if (req.getMethod() == "GET") {
            Get(req);
        } else if (req.getMethod() == "POST") {
            Post(req);
        } else if (req.getMethod() == "PUT") {
            Put(req);
        }
        else if (req.getMethod() == "PATCH") {
            Patch(req);
        }
        else if (req.getMethod() == "DELETE") {
            Delete(req);
        }
        else {
            sendNotFound();
        }
    }

    virtual void Get(HttpRequest& req) {
        sendNotFound();
    };
    virtual void Post(HttpRequest& req) {
        sendNotFound();
    };
    virtual void Put(HttpRequest& req) {
        sendNotFound();
    };
    virtual void Patch(HttpRequest& req) {
        sendNotFound();
    };
    virtual void Delete(HttpRequest& req) {
        sendNotFound();
    };
};
#endif