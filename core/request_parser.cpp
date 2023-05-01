#include "request_parser.hpp"

HttpRequestParser::HttpRequestParser()
{
    /* Initialize user callbacks and settings */
    llhttp_settings_init(&settings_);

    settings_.on_url = on_url;
    settings_.on_header_field = on_header_field;
    settings_.on_header_value = on_header_value;
    settings_.on_method_complete = on_method_complete;
    settings_.on_version_complete = on_version_complete;

    llhttp_init(&parser_, HTTP_BOTH, &settings_);
    parser_.data = this;
}

size_t HttpRequestParser::parse(const char *data, size_t len)
{
    enum llhttp_errno err = llhttp_execute(&parser_, data, len);
    if (err == HPE_OK)
    {
        /* Successfully parsed! */
    }
    else
    {
        fprintf(stderr, "Parse error: %s %s\n", llhttp_errno_name(err),
                parser_.reason);
    }
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
    HttpRequestParser *p = static_cast<HttpRequestParser *>(parser->data);

    p->current_header_field_ = std::string(at, length);

    return 0;
}

int HttpRequestParser::on_version_complete(llhttp_t *parser)
{
    HttpRequestParser *p = static_cast<HttpRequestParser *>(parser->data);
    p->version_major_ = parser->http_major;
    p->version_minor_ = parser->http_minor;

    return 0;
}

int HttpRequestParser::on_method_complete(llhttp_t *parser)
{
    HttpRequestParser *p = static_cast<HttpRequestParser *>(parser->data);
    llhttp_method_t method = static_cast<llhttp_method_t>(parser->method);

    switch (method) {
        // @todo: Change method names to const strings instead of literals
        case HTTP_DELETE:
            p->method_ = "DELETE";
            break;
        case HTTP_GET:
            p->method_ = "GET";
            break;
        case HTTP_POST:
            p->method_ = "POST";
            break;
        case HTTP_PATCH:
            p->method_ = "PATCH";
            break;
        case HTTP_PUT:
            p->method_ = "PUT";
            break;
        default:
            p->method_ = "Unknown";
            break;
    }

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

HttpRequest HttpRequestParser::getRequest() const
{
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