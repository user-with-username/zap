#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <httplib.h>

#include "../http/request.h"
#include "../http/response.h"
#include "params.h"

namespace zap {

struct SourceInfo {
    const char* file_name     = "unknown";
    const char* function_name = "unknown";
    uint32_t    line          = 0;
};

struct IHandlerInvoker {
    std::string registered_path;
    SourceInfo  source;

    virtual ~IHandlerInvoker() = default;
    virtual HandlerResult invoke(const httplib::Request& req,
                                 const std::vector<std::string>& matches) = 0;
};

template <typename Func>
struct RealInvoker : public IHandlerInvoker {
    Func func;

    explicit RealInvoker(Func&& f) : func(std::forward<Func>(f)) {}

    HandlerResult invoke(const httplib::Request& req,
                         const std::vector<std::string>& matches) override {
        Request zap_req(req, matches);
        return dispatch(zap_req, matches);
    }

private:
    HandlerResult dispatch(Request& req, const std::vector<std::string>& m) {
        if constexpr (std::is_invocable_v<Func, Request&>)
            return HandlerResult(func(req));

        else if constexpr (std::is_invocable_v<Func>)
            return HandlerResult(func());

        else if constexpr (std::is_invocable_v<Func, std::string>)
            return HandlerResult(func(m[0]));

        else if constexpr (std::is_invocable_v<Func, int>)
            return HandlerResult(func(convert_param<int>(m[0], 1)));

        else if constexpr (std::is_invocable_v<Func, double>)
            return HandlerResult(func(convert_param<double>(m[0], 1)));

        else if constexpr (std::is_invocable_v<Func, std::string, std::string>)
            return HandlerResult(func(m[0], m[1]));

        else if constexpr (std::is_invocable_v<Func, int, int>)
            return HandlerResult(func(convert_param<int>(m[0], 1),
                                      convert_param<int>(m[1], 2)));

        else if constexpr (std::is_invocable_v<Func, int, std::string>)
            return HandlerResult(func(convert_param<int>(m[0], 1), m[1]));

        else if constexpr (std::is_invocable_v<Func, std::string, int>)
            return HandlerResult(func(m[0], convert_param<int>(m[1], 2)));

        else if constexpr (std::is_invocable_v<Func, Request&, std::string>)
            return HandlerResult(func(req, m[0]));

        else if constexpr (std::is_invocable_v<Func, Request&, int>)
            return HandlerResult(func(req, convert_param<int>(m[0], 1)));

        else if constexpr (std::is_invocable_v<Func, Request&, std::string, std::string>)
            return HandlerResult(func(req, m[0], m[1]));

        else if constexpr (std::is_invocable_v<Func, Request&, int, int>)
            return HandlerResult(func(req, convert_param<int>(m[0], 1),
                                           convert_param<int>(m[1], 2)));

        else
            static_assert(sizeof(Func) == 0, "Unsupported handler signature");
    }
};

class Handler {
public:
    Handler() = default;

    template <typename Func>
    Handler(Func&& func) {
        invoker_ = std::make_shared<RealInvoker<std::decay_t<Func>>>(std::forward<Func>(func));
    }

    Handler(const Handler&)            = default;
    Handler& operator=(const Handler&) = default;
    Handler(Handler&&)                 = default;

    HandlerResult invoke(const httplib::Request& req,
                         const std::vector<std::string>& matches) const {
        if (!invoker_) throw std::runtime_error("Handler not initialized");
        return invoker_->invoke(req, matches);
    }

    void set_source(const SourceInfo& src)    { if (invoker_) invoker_->source          = src;  }
    void set_path(const std::string& path)    { if (invoker_) invoker_->registered_path = path; }

    IHandlerInvoker* get_invoker() const { return invoker_.get(); }

private:
    std::shared_ptr<IHandlerInvoker> invoker_;
};

} // namespace zap
