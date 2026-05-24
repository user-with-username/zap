#pragma once

#include <set>
#include <string>

namespace zap {

    struct CorsConfig {
    bool enabled           = false;
    bool allow_credentials = false;
    int  max_age           = 86400;

    std::set<std::string> allowed_origins;
    std::set<std::string> allowed_methods = {"GET", "POST", "PUT", "DELETE", "PATCH", "OPTIONS"};
    std::set<std::string> allowed_headers = {"Content-Type", "Authorization", "X-Requested-With"};

    bool is_origin_allowed(const std::string& origin) const {
        if (allowed_origins.empty())         return true;
        if (allowed_origins.count("*") > 0)  return true;
        return allowed_origins.count(origin) > 0;
    }

    std::string allowed_methods_str() const { return join(allowed_methods); }
    std::string allowed_headers_str() const { return join(allowed_headers); }

private:
    static std::string join(const std::set<std::string>& items) {
        std::string result;
        for (const auto& item : items) {
            if (!result.empty()) result += ", ";
            result += item;
        }
        return result;
    }
};

} // namespace zap
