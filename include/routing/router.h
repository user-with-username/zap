#pragma once

#include <shared_mutex>
#include <vector>
#include "route.h"

namespace zap {

class Router {
public:
    void add_route(Route route);
    const Route* find_route(const std::string& method, const std::string& path,
                            std::vector<std::string>& out_params) const;
    
    const std::vector<Route>& all_routes() const { return routes_; }
    void clear();
    size_t route_count() const;
    
private:
    std::string to_regex_path(const std::string& path) const;
    
    std::vector<Route> routes_;
    mutable std::shared_mutex mutex_;
};

} // namespace zap