#pragma once

#include <string>
#include <vector>

#include "../middleware.h"
#include "route_builder.h"

namespace zap {

class App;
class Group {
public:
    Group(App* app, const std::string& prefix);

    Group& use(Middleware mw);

    RouteBuilder get(const std::string& path);
    RouteBuilder post(const std::string& path);
    RouteBuilder put(const std::string& path);
    RouteBuilder del(const std::string& path);
    RouteBuilder patch(const std::string& path);

    template <typename F>
    void group(const std::string& path, F&& builder) {
        Group subgroup(app_, prefix_ + path);
        subgroup.middlewares_ = middlewares_;
        builder(subgroup);
    }

private:
    RouteBuilder make_builder(const std::string& method, const std::string& path);

    App*                   app_;
    std::string            prefix_;
    std::vector<Middleware> middlewares_;
};

} // namespace zap
