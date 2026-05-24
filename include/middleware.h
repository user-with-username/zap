#pragma once

#include <functional>
#include "http/request.h"

namespace zap {

using NextFunction = std::function<void()>;
using Middleware   = std::function<void(Request&, NextFunction)>;

} // namespace zap
