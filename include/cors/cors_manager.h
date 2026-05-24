#pragma once

#include <string>
#include <unordered_set>
#include <httplib.h>

namespace zap {

struct CorsConfig {
    bool enabled = false;
    std::unordered_set<std::string> allowed_origins;
    std::unordered_set<std::string> allowed_methods;
    std::unordered_set<std::string> allowed_headers;
    bool allow_credentials = false;
    int max_age = 86400;  // 24 hours

    bool is_origin_allowed(const std::string& origin) const;
    std::string allowed_methods_str() const;
    std::string allowed_headers_str() const;
};

class CorsManager {
public:
    explicit CorsManager(CorsConfig config);
    
    void apply_headers(httplib::Response& res, const std::string& origin) const;
    void apply_preflight(httplib::Response& res, const std::string& origin,
                         const std::string& request_method,
                         const std::string& request_headers) const;
    
    bool is_enabled() const { return config_.enabled; }
    void update_config(const CorsConfig& config) { config_ = config; }
    
private:
    CorsConfig config_;
};

} // namespace zap