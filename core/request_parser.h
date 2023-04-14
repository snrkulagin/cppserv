#ifndef HTTP_REQUEST_PARSER_H
#define HTTP_REQUEST_PARSER_H

#include <string>
#include "llhttp.h"
#include "http_request.h"

class HttpRequestParser {
public:
    HttpRequestParser();

    size_t parse(const char* data, size_t len);

    std::string method() const;
    std::string path() const;
    int version_major() const;
    int version_minor() const;
    std::string user_agent() const;
    std::string cookie() const;

    HttpRequest getRequest() const;

private:
    static int on_message_begin(llhttp_t* parser);
    static int on_url(llhttp_t* parser, const char* at, size_t length);
    static int on_header_field(llhttp_t* parser, const char* at, size_t length);
    static int on_header_value(llhttp_t* parser, const char* at, size_t length);

    std::string current_header_field_;

    llhttp_t parser_;

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
};

#endif /* HTTP_REQUEST_PARSER_H */