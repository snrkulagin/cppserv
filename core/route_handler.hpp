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
    void sendResponse(HttpResponse& resp) {
        sender->sendResponse(resp);
    };

    std::shared_ptr<Sender> sender;

    void injectSender(std::shared_ptr<Sender>& sender) {
        this->sender = sender;
    };

    virtual ~BaseRouteHandler() {}

    virtual void Handle(HttpRequest& req) {
        path = req.getPath();

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