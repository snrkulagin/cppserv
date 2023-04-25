#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>

class HttpRequest {
private:
    std::string method_;
    std::string path_;
    int version_major_;
    int version_minor_;
    std::string user_agent_;
    std::string cookie_;
    std::string acceptEncoding_;
    std::string connection_;
    std::string contentType_;
    std::string contentLength_;

public:
    // default constructor
    HttpRequest() {
        method_ = "";
        path_ = "";
        version_major_ = 0;
        version_minor_ = 0;
        user_agent_ = "";
        cookie_ = "";
        acceptEncoding_ = "";
        connection_ = "";
        contentType_ = "";
        contentLength_ = "";
    }

    // getters and setters for method_
    std::string getMethod() const {
        return method_;
    }

    HttpRequest& setMethod(std::string method) {
        method_ = method;
        return *this;
    }

    // getters and setters for path_
    std::string getPath() const {
        return path_;
    }

    HttpRequest& setPath(std::string path) {
        path_ = path;
        return *this;
    }

    // getters and setters for version_major_
    int getVersionMajor() const {
        return version_major_;
    }

    HttpRequest& setVersionMajor(int version_major) {
        version_major_ = version_major;
        return *this;
    }

    // getters and setters for version_minor_
    int getVersionMinor() const {
        return version_minor_;
    }

    HttpRequest& setVersionMinor(int version_minor) {
        version_minor_ = version_minor;
        return *this;
    }

    // getters and setters for user_agent_
    std::string getUserAgent() const {
        return user_agent_;
    }

    HttpRequest& setUserAgent(std::string user_agent) {
        user_agent_ = user_agent;
        return *this;
    }

    // getters and setters for cookie_
    std::string getCookie() const {
        return cookie_;
    }

    HttpRequest& setCookie(std::string cookie) {
        cookie_ = cookie;
        return *this;
    }

    // getters and setters for acceptEncoding_
    std::string getAcceptEncoding() const {
        return acceptEncoding_;
    }

    HttpRequest& setAcceptEncoding(std::string acceptEncoding) {
        acceptEncoding_ = acceptEncoding;
        return *this;
    }

    // getters and setters for connection_
    std::string getConnection() const {
        return connection_;
    }

    HttpRequest& setConnection(std::string connection) {
        connection_ = connection;
        return *this;
    }

    // getters and setters for contentType_
    std::string getContentType() const {
        return contentType_;
    }

    HttpRequest& setContentType(std::string contentType) {
        contentType_ = contentType;
        return *this;
    }

    // getters and setters for contentLength_
    std::string getContentLength() const {
        return contentLength_;
    }

    HttpRequest& setContentLength(std::string contentLength) {
        contentLength_ = contentLength;
        return *this;
    }
};
#endif