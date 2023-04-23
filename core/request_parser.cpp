#include "request_parser.hpp"

HttpRequestParser::HttpRequestParser()
{
    llhttp_settings_t settings;

    /* Initialize user callbacks and settings */
    llhttp_settings_init(&settings);

    llhttp_init(&parser_, HTTP_REQUEST, &settings);
    parser_.data = this;
}

size_t HttpRequestParser::parse(const char *data, size_t len)
{
    size_t nparsed = llhttp_execute(&parser_, data, len);
    if (parser_.upgrade) {} // ignore
    return nparsed;
}

int HttpRequestParser::on_message_begin(llhttp_t *parser)
{
    HttpRequestParser *p = static_cast<HttpRequestParser *>(parser->data);
    p->method_ = llhttp_method_name(static_cast<llhttp_method_t>(parser->method));
    p->version_major_ = parser->http_major;
    p->version_minor_ = parser->http_minor;
    return 0;
}

int HttpRequestParser::on_url(llhttp_t *parser, const char *at, size_t length)
{
    HttpRequestParser *p = static_cast<HttpRequestParser *>(parser->data);
    p->path_ = std::string(at, length);
    return 0;
}

int HttpRequestParser::on_header_field(llhttp_t *parser, const char *at, size_t length)
{
    return 0;
}

int HttpRequestParser::on_header_value(llhttp_t *parser, const char *at, size_t length)
{
    HttpRequestParser *p = static_cast<HttpRequestParser *>(parser->data);
    
    if (p->current_header_field_ == "User-Agent")
    {
        p->user_agent_ = std::string(at, length);
    }

    if (p->current_header_field_ == "Cookie")
    {
        p->cookie_ = std::string(at, length);
    }

    if (p->current_header_field_ == "Accept-Encoding")
    {
        p->acceptEncoding_ = std::string(at, length);
    }

    if (p->current_header_field_ == "Connection")
    {
        p->connection_ = std::string(at, length);
    }

    if (p->current_header_field_ == "Content-Type")
    {
        p->contentType_ = std::string(at, length);
    }

    if (p->current_header_field_ == "Content-Length")
    {
        p->contentLength_ = std::string(at, length);
    }
    return 0;
}

HttpRequest HttpRequestParser::getRequest() const {
    HttpRequest req;

    return req.setAcceptEncoding(acceptEncoding_)
        .setConnection(connection_)  
        .setCookie(cookie_)
        .setContentType(contentType_)
        .setContentLength(contentLength_)
        .setUserAgent(user_agent_)
        .setVersionMinor(version_minor_)
        .setVersionMajor(version_major_)
        .setPath(path_)
        .setMethod(method_);
}