#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <sstream>
#include <unordered_map>

class HttpResponse {
public:
    HttpResponse() : status_code_(200), status_text_("OK") {}

    HttpResponse& setHeader(const std::string& name, const std::string& value) {
        headers_[name] = value;
        return *this;
    }

    HttpResponse& setStatusCode(int code) {
        status_code_ = code;
        return *this;
    }

    HttpResponse& setStatusText(const std::string& text) {
        status_text_ = text;
        return *this;
    }

    HttpResponse& setJsonData(const std::string& json_data) {
        response_data_ = json_data;
        setHeader("Content-Type", "application/json");
        setHeader("Content-Length", std::to_string(response_data_.size()));
        return *this;
    }

    HttpResponse& setTextData(const std::string& text_data) {
        response_data_ = text_data;
        setHeader("Content-Type", "text/plain");
        setHeader("Content-Length", std::to_string(response_data_.size()));
        return *this;
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << "HTTP/1.1 " << status_code_ << " " << status_text_ << "\r\n";
        for (const auto& header : headers_) {
            oss << header.first << ": " << header.second << "\r\n";
        }
        oss << "\r\n";
        if (!response_data_.empty()) {
            oss << response_data_;
        }
        return oss.str();
    }

private:
    int status_code_;
    std::string status_text_;
    std::string response_data_;
    std::unordered_map<std::string, std::string> headers_;
};
#endif