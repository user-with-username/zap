#pragma once

#include <source_location>
#include <string>
#include <vector>

#include "../middleware.h"

namespace zap {

class App;

class RouteBuilder {
public:
    RouteBuilder(App* app, const std::string& method, const std::string& path);

    RouteBuilder& middleware(Middleware mw);

    template <typename HandlerFunc>
    void handle(HandlerFunc&& handler,
                std::source_location loc = std::source_location::current()) {
        add_handler(std::forward<HandlerFunc>(handler), loc);
    }

    template <typename HandlerFunc>
    void operator()(HandlerFunc&& handler,
                    std::source_location loc = std::source_location::current()) {
        add_handler(std::forward<HandlerFunc>(handler), loc);
    }

private:
    template <typename HandlerFunc>
    void add_handler(HandlerFunc&& handler, std::source_location loc);

    App*                   app_;
    std::string            method_;
    std::string            path_;
    std::vector<Middleware> middlewares_;
};

} // namespace zap
