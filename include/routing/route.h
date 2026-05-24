#pragma once

#include <regex>
#include <string>
#include <vector>

#include "../middleware.h"
#include "handler.h"

namespace zap {
struct Route {
    std::string            method;
    std::string            path;
    std::regex             regex;
    Handler                handler;
    std::vector<Middleware> middlewares;
};

} // namespace zap
