#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "status.h"
#include "content_type.h"

namespace zap {

using json = nlohmann::json;

class Response {
public:
    Response() = default;

    Response& status(HttpStatus s)                              { status_ = s;                  return *this; }
    Response& header(const std::string& key, const std::string& value) { headers_[key] = value; return *this; }

    Response& json(const ::zap::json& data) {
        body_         = data.dump();
        content_type_ = ContentType::kJson;
        return *this;
    }

    Response& text(const std::string& t) {
        body_         = t;
        content_type_ = ContentType::kText;
        return *this;
    }

    Response& html(const std::string& h) {
        body_         = h;
        content_type_ = ContentType::kHtml;
        return *this;
    }

    void apply(httplib::Response& res) const {
        res.status = to_code(status_);
        for (const auto& [key, value] : headers_) res.set_header(key, value);
        res.set_content(body_, content_type_);
    }

    const std::string& body()         const { return body_; }
    const std::string& content_type() const { return content_type_; }
    HttpStatus         status()        const { return status_; }

private:
    HttpStatus  status_       = HttpStatus::OK;
    std::string body_;
    std::string content_type_ = ContentType::kJson;
    std::unordered_map<std::string, std::string> headers_;
};

struct HandlerResult {
    std::string body;
    std::string content_type;
    HttpStatus  status = HttpStatus::OK;
    std::unordered_map<std::string, std::string> headers;

    HandlerResult() : content_type(ContentType::kJson) {}

    HandlerResult(std::string_view t) : body(t) {
        if ((t.starts_with('{') && t.ends_with('}')) || (t.starts_with('[') && t.ends_with(']'))) {
            content_type = ContentType::kJson;
        } else if (t.find("<html") != std::string_view::npos || t.find("<!DOCTYPE html") != std::string_view::npos) {
            content_type = ContentType::kHtml;
        } else {
            content_type = ContentType::kText;
        }
    }

    HandlerResult(const char* t) : HandlerResult(std::string_view(t)) {}
    HandlerResult(const json& j) : body(j.dump()), content_type(ContentType::kJson) {}
    HandlerResult(const Response& resp) : body(resp.body()), content_type(resp.content_type()), status(resp.status()) {}

    void apply(httplib::Response& res) const {
        res.status = to_code(status);
        for (const auto& [key, value] : headers) res.set_header(key, value);
        res.set_content(body, content_type);
    }
};

class HttpException : public std::runtime_error {
public:
    HttpException(HttpStatus status, const char* message)
        : std::runtime_error(message), status_(status) {}

    HttpException(HttpStatus status, std::string message)
        : std::runtime_error(std::move(message)), status_(status) {}

    HttpException(HttpStatus status, const json& error_body)
        : std::runtime_error(error_body.dump()), status_(status),
          error_body_(error_body), has_custom_body_(true) {}

    HttpStatus  status()          const { return status_; }
    const json& error_body()      const { return error_body_; }
    bool        has_custom_body() const { return has_custom_body_; }

private:
    HttpStatus status_;
    json       error_body_;
    bool       has_custom_body_ = false;
};

} // namespace zap