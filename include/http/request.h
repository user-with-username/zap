#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include <httplib.h>
#include <nlohmann/json.hpp>

namespace zap {

using json = nlohmann::json;

class Request {
public:
    explicit Request(const httplib::Request& req, std::vector<std::string> matches = {})
        : req_(req), matches_(std::move(matches)) {}

    json json_body() const {
        if (req_.body.empty()) return json::object();
        return json::parse(req_.body);
    }

    json json() const { return json_body(); }


    const std::vector<std::string>& matches() const { return matches_; }

    std::string match(size_t index) const {
        return (index < matches_.size()) ? matches_[index] : "";
    }

    std::optional<std::string> query(const std::string& name) const {
        if (req_.has_param(name)) return req_.get_param_value(name);
        return std::nullopt;
    }

    template <typename T>
    T query(const std::string& name, T default_value) const {
        auto val = query(name);
        if (!val) return default_value;

        try {
            if constexpr (std::is_same_v<T, std::string>) {
                return *val;
            } else if constexpr (std::is_same_v<T, int>) {
                return std::stoi(*val);
            } else if constexpr (std::is_same_v<T, long long>) {
                return std::stoll(*val);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::stod(*val);
            } else if constexpr (std::is_same_v<T, bool>) {
                return *val == "true" || *val == "1" || *val == "yes";
            }
        } catch (...) {}

        return default_value;
    }

    std::string header(const std::string& name)     const { return req_.get_header_value(name); }
    std::string get_header(const std::string& name) const { return req_.get_header_value(name); }

    const httplib::Request& raw()    const { return req_; }
    std::string             path()   const { return req_.path; }
    std::string             method() const { return req_.method; }

private:
    const httplib::Request&  req_;
    std::vector<std::string> matches_;
};

} // namespace zap
