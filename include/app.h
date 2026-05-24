#pragma once

#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <regex>
#include <shared_mutex>
#include <source_location>
#include <string>
#include <thread>
#include <vector>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "cors/cors.h"
#include "http/content_type.h"
#include "http/request.h"
#include "http/response.h"
#include "http/status.h"
#include "middleware.h"
#include "routing/group.h"
#include "routing/handler.h"
#include "routing/params.h"
#include "routing/route.h"
#include "routing/route_builder.h"

namespace zap {

class App {
public:
    App();
    ~App();

    App& threads(int n);

    App& enable_cors();
    App& disable_cors();
    App& allowed_origins(std::initializer_list<std::string> origins);
    App& allowed_methods(std::initializer_list<std::string> methods);
    App& allowed_headers(std::initializer_list<std::string> headers);
    App& allow_credentials(bool allow);

    App& use(Middleware mw);

    RouteBuilder get(const std::string& path);
    RouteBuilder post(const std::string& path);
    RouteBuilder put(const std::string& path);
    RouteBuilder del(const std::string& path);
    RouteBuilder patch(const std::string& path);

    template <typename F>
    void group(const std::string& prefix, F&& builder) {
        Group g(this, prefix);
        builder(g);
    }

    void listen(int port);

    void start(int port, const std::string& host = "0.0.0.0");

    void serve(int port, const std::string& host = "0.0.0.0");

    void stop();
    bool is_running() const;

    void add_route(const std::string& method, const std::string& path,
                   const std::vector<Middleware>& mws, Handler handler,
                   std::source_location loc);

private:
    void configure_server();
    std::string to_regex_path(const std::string& path) const;

    void handle_request(const httplib::Request& req, httplib::Response& res);
    void execute_handler(const httplib::Request& req, httplib::Response& res,
                         const Route& route, const std::vector<std::string>& params);

    std::vector<Middleware> build_middleware_pipeline(const Route& route) const;
    void run_pipeline(const std::vector<Middleware>& pipeline, Request& req,
                      std::function<void()> final_handler);

    void handle_no_route(const httplib::Request& req, httplib::Response& res,
                         bool path_matched);
    void handle_http_exception(const httplib::Request& req, httplib::Response& res,
                               const HttpException& e);
    void handle_parameter_exception(const httplib::Request& req, httplib::Response& res,
                                    const ParameterCastException& e, const Handler& handler);
    void handle_std_exception(const httplib::Request& req, httplib::Response& res,
                              const std::exception& e, const Handler& handler);

    void handle_cors_preflight(const httplib::Request& req, httplib::Response& res);
    void set_cors_headers(httplib::Response& res, const std::string& origin) const;
    void apply_cors_headers(const httplib::Request& req, httplib::Response& res) const;

    void log_request(const httplib::Request& req, HttpStatus status,
                     const std::string& note = "") const;
    void log_error(const httplib::Request& req, const std::string& error) const;

    std::unique_ptr<httplib::Server> server_;
    std::vector<Route>               routes_;
    mutable std::shared_mutex        routes_mutex_;
    std::vector<Middleware>          global_middlewares_;
    CorsConfig                       cors_;
    int                              thread_pool_size_;
    std::thread                      server_thread_;
    std::atomic<bool>                running_;
};

inline RouteBuilder::RouteBuilder(App* app, const std::string& method, const std::string& path)
    : app_(app), method_(method), path_(path) {}

inline RouteBuilder& RouteBuilder::middleware(Middleware mw) {
    middlewares_.push_back(std::move(mw));
    return *this;
}

template <typename HandlerFunc>
inline void RouteBuilder::add_handler(HandlerFunc&& handler, std::source_location loc) {
    app_->add_route(method_, path_, middlewares_,
                    Handler(std::forward<HandlerFunc>(handler)), loc);
}

inline Group::Group(App* app, const std::string& prefix)
    : app_(app), prefix_(prefix) {}

inline Group& Group::use(Middleware mw) {
    middlewares_.push_back(std::move(mw));
    return *this;
}

inline RouteBuilder Group::make_builder(const std::string& method, const std::string& path) {
    RouteBuilder builder(app_, method, prefix_ + path);
    for (auto& mw : middlewares_) builder.middleware(mw);
    return builder;
}

inline RouteBuilder Group::get(const std::string& path)   { return make_builder("GET",    path); }
inline RouteBuilder Group::post(const std::string& path)  { return make_builder("POST",   path); }
inline RouteBuilder Group::put(const std::string& path)   { return make_builder("PUT",    path); }
inline RouteBuilder Group::del(const std::string& path)   { return make_builder("DELETE", path); }
inline RouteBuilder Group::patch(const std::string& path) { return make_builder("PATCH",  path); }

inline App::App() : server_(std::make_unique<httplib::Server>()), running_(false) {
    thread_pool_size_ = static_cast<int>(std::thread::hardware_concurrency());
    if (thread_pool_size_ == 0) thread_pool_size_ = 4;
}

inline App::~App() {
    stop();
}

inline App& App::threads(int n)                                       { if (n > 0) thread_pool_size_ = n; return *this; }
inline App& App::enable_cors()                                        { cors_.enabled = true;  return *this; }
inline App& App::disable_cors()                                       { cors_.enabled = false; return *this; }
inline App& App::allow_credentials(bool allow)                        { cors_.allow_credentials = allow; return *this; }
inline App& App::use(Middleware mw)                                   { global_middlewares_.push_back(std::move(mw)); return *this; }

inline App& App::allowed_origins(std::initializer_list<std::string> origins) {
    cors_.allowed_origins = {origins.begin(), origins.end()};
    return *this;
}

inline App& App::allowed_methods(std::initializer_list<std::string> methods) {
    cors_.allowed_methods = {methods.begin(), methods.end()};
    return *this;
}

inline App& App::allowed_headers(std::initializer_list<std::string> headers) {
    cors_.allowed_headers = {headers.begin(), headers.end()};
    return *this;
}

inline RouteBuilder App::get(const std::string& path)   { return RouteBuilder(this, "GET",    path); }
inline RouteBuilder App::post(const std::string& path)  { return RouteBuilder(this, "POST",   path); }
inline RouteBuilder App::put(const std::string& path)   { return RouteBuilder(this, "PUT",    path); }
inline RouteBuilder App::del(const std::string& path)   { return RouteBuilder(this, "DELETE", path); }
inline RouteBuilder App::patch(const std::string& path) { return RouteBuilder(this, "PATCH",  path); }

inline void App::listen(int port) {
    start(port);
    std::cin.get();
    stop();
}

inline void App::start(int port, const std::string& host) {
    if (running_) return;
    running_      = true;
    server_thread_ = std::thread([this, port, host]() {
        configure_server();
        std::cout << "Zap server listening on http://" << host << ":" << port << "\n";
        server_->listen(host.c_str(), port);
        running_ = false;
    });
}

inline void App::serve(int port, const std::string& host) {
    configure_server();
    std::cout << "Zap server listening on http://" << host << ":" << port << "\n";
    server_->listen(host.c_str(), port);
}

inline void App::stop() {
    if (!running_) return;
    server_->stop();
    if (server_thread_.joinable()) server_thread_.join();
    running_ = false;
}

inline bool App::is_running() const { return running_; }

inline void App::add_route(const std::string& method, const std::string& path,
                           const std::vector<Middleware>& mws, Handler handler,
                           std::source_location loc) {
    SourceInfo src{loc.file_name(), loc.function_name(), static_cast<uint32_t>(loc.line())};
    handler.set_source(src);
    handler.set_path(path);

    Route route;
    route.method      = method;
    route.path        = path;
    route.regex       = std::regex(to_regex_path(path));
    route.handler     = std::move(handler);
    route.middlewares = mws;

    std::unique_lock lock(routes_mutex_);
    routes_.push_back(std::move(route));
}

inline std::string App::to_regex_path(const std::string& path) const {
    std::string result = path;
    result = std::regex_replace(result, std::regex(R"(/:([^/]+))"),   "/([^/]+)");
    result = std::regex_replace(result, std::regex(R"(\{([^}]+)\})"), "([^/]+)");
    return "^" + result + "/?$";
}

inline void App::configure_server() {
    server_->new_task_queue = [this] {
        return new httplib::ThreadPool(thread_pool_size_);
    };

    auto catch_all = [this](const httplib::Request& req, httplib::Response& res) {
        if (cors_.enabled && req.method == "OPTIONS") {
            handle_cors_preflight(req, res);
            return;
        }
        handle_request(req, res);
    };

    server_->Get(R"(/.*)",     catch_all);
    server_->Post(R"(/.*)",    catch_all);
    server_->Put(R"(/.*)",     catch_all);
    server_->Delete(R"(/.*)",  catch_all);
    server_->Patch(R"(/.*)",   catch_all);
    server_->Options(R"(/.*)", catch_all);
}

inline void App::handle_request(const httplib::Request& req, httplib::Response& res) {
    bool path_matched = false;

    std::vector<Route> snapshot;
    {
        std::shared_lock lock(routes_mutex_);
        snapshot = routes_;
    }

    for (const auto& route : snapshot) {
        std::smatch matches;
        if (!std::regex_match(req.path, matches, route.regex)) continue;

        path_matched = true;
        if (route.method != req.method) continue;

        std::vector<std::string> params;
        for (size_t i = 1; i < matches.size(); ++i) params.push_back(matches[i].str());

        try {
            execute_handler(req, res, route, params);
        } catch (const HttpException& e) {
            handle_http_exception(req, res, e);
        } catch (const ParameterCastException& e) {
            handle_parameter_exception(req, res, e, route.handler);
        } catch (const std::exception& e) {
            handle_std_exception(req, res, e, route.handler);
        }
        return;
    }

    handle_no_route(req, res, path_matched);
}

inline void App::execute_handler(const httplib::Request& req, httplib::Response& res,
                                 const Route& route, const std::vector<std::string>& params) {
    Request zap_req(req, params);
    auto pipeline = build_middleware_pipeline(route);

    run_pipeline(pipeline, zap_req, [&]() {
        HandlerResult result = route.handler.invoke(req, params);
        result.apply(res);
        apply_cors_headers(req, res);
        log_request(req, result.status);
    });
}

inline std::vector<Middleware> App::build_middleware_pipeline(const Route& route) const {
    std::vector<Middleware> pipeline;
    pipeline.reserve(global_middlewares_.size() + route.middlewares.size());
    pipeline.insert(pipeline.end(), global_middlewares_.begin(), global_middlewares_.end());
    pipeline.insert(pipeline.end(), route.middlewares.begin(),   route.middlewares.end());
    return pipeline;
}

inline void App::run_pipeline(const std::vector<Middleware>& pipeline,
                              Request& req, std::function<void()> final_handler) {
    size_t idx = 0;
    std::function<void()> next;
    next = [&]() {
        if (idx < pipeline.size()) {
            auto mw = pipeline[idx++];
            mw(req, next);
        } else {
            final_handler();
        }
    };
    next();
}

inline void App::handle_http_exception(const httplib::Request& req, httplib::Response& res,
                                       const HttpException& e) {
    res.status = to_code(e.status());

    if (e.has_custom_body()) {
        res.set_content(e.error_body().dump(), ContentType::kJson);
    } else {
        res.set_content(json{{"error", e.what()}}.dump(), ContentType::kJson);
    }

    apply_cors_headers(req, res);
    log_request(req, e.status(), e.what());
}

inline void App::handle_parameter_exception(const httplib::Request& req, httplib::Response& res,
                                            const ParameterCastException& e, const Handler& handler) {
    auto* invoker = handler.get_invoker();
    if (!invoker) return;

    std::string filepath(invoker->source.file_name);
    size_t pos = filepath.find("src/");
    if (pos == std::string::npos) pos = filepath.find("src\\");
    std::string short_path = (pos != std::string::npos) ? filepath.substr(pos) : filepath;

    std::cerr << "error: type mismatch in " << req.method << " " << req.path << "\n"
              << "  param " << e.arg_index() << " should be " << e.expected_type()
              << ", got \"" << e.got_value() << "\"\n"
              << "  at " << short_path << ":" << invoker->source.line
              << " (" << invoker->registered_path << ")\n";

    res.status = to_code(HttpStatus::BadRequest);
    res.set_content(json{{"error", "Bad Request"}, {"message", e.what()}}.dump(), ContentType::kJson);
    apply_cors_headers(req, res);
    log_request(req, HttpStatus::BadRequest, e.what());
}

inline void App::handle_std_exception(const httplib::Request& req, httplib::Response& res,
                                      const std::exception& e, const Handler& handler) {
    auto* invoker = handler.get_invoker();
    if (!invoker) return;

    std::string filepath(invoker->source.file_name);
    size_t pos = filepath.find("src/");
    if (pos == std::string::npos) pos = filepath.find("src\\");
    std::string short_path = (pos != std::string::npos) ? filepath.substr(pos) : filepath;

    std::cerr << "error: " << e.what() << "\n"
              << "  at " << short_path << ":" << invoker->source.line << "\n";

    res.status = to_code(HttpStatus::InternalServerError);
    res.set_content(json{{"error", "Internal Server Error"}}.dump(), ContentType::kJson);
    apply_cors_headers(req, res);
    log_error(req, e.what());
}

inline void App::handle_no_route(const httplib::Request& req, httplib::Response& res,
                                 bool path_matched) {
    if (path_matched) {
        res.status = to_code(HttpStatus::MethodNotAllowed);
        res.set_content(json{
            {"error", "Method Not Allowed"},
            {"method", req.method},
            {"path",   req.path}
        }.dump(), ContentType::kJson);
        log_request(req, HttpStatus::MethodNotAllowed);
    } else {
        res.status = to_code(HttpStatus::NotFound);
        res.set_content(json{{"error", "Not Found"}, {"path", req.path}}.dump(), ContentType::kJson);
        log_request(req, HttpStatus::NotFound);
    }
    apply_cors_headers(req, res);
}

inline void App::handle_cors_preflight(const httplib::Request& req, httplib::Response& res) {
    std::string origin = req.get_header_value("Origin");

    if (!origin.empty() && cors_.is_origin_allowed(origin)) {
        set_cors_headers(res, origin);

        if (const auto method = req.get_header_value("Access-Control-Request-Method"); !method.empty())
            res.set_header("Access-Control-Allow-Methods", cors_.allowed_methods_str());

        if (const auto headers = req.get_header_value("Access-Control-Request-Headers"); !headers.empty())
            res.set_header("Access-Control-Allow-Headers", cors_.allowed_headers_str());

        res.set_header("Access-Control-Max-Age", std::to_string(cors_.max_age));
    }

    res.status = to_code(HttpStatus::NoContent);
    res.set_content("", ContentType::kText);
    log_request(req, HttpStatus::NoContent);
}

inline void App::set_cors_headers(httplib::Response& res, const std::string& origin) const {
    if (cors_.allowed_origins.count("*") > 0 && !cors_.allow_credentials) {
        res.set_header("Access-Control-Allow-Origin", "*");
    } else {
        res.set_header("Access-Control-Allow-Origin", origin);
    }

    if (cors_.allow_credentials)
        res.set_header("Access-Control-Allow-Credentials", "true");

    res.set_header("Access-Control-Expose-Headers", "Content-Length, Content-Type");
}

inline void App::apply_cors_headers(const httplib::Request& req, httplib::Response& res) const {
    if (!cors_.enabled) return;
    std::string origin = req.get_header_value("Origin");
    if (!origin.empty() && cors_.is_origin_allowed(origin)) set_cors_headers(res, origin);
}

inline void App::log_request(const httplib::Request& req, HttpStatus status,
                             const std::string& note) const {
    std::cout << (is_success(status) ? "✓" : "✗") << " "
              << req.method << " " << req.path
              << " → " << to_code(status) << " " << to_string(status);
    if (!note.empty()) std::cout << " (" << note << ")";
    std::cout << "\n";
}

inline void App::log_error(const httplib::Request& req, const std::string& error) const {
    std::cerr << "✗ Error in " << req.method << " " << req.path << ": " << error << "\n";
}

inline App& get_app() {
    static App instance;
    return instance;
}

} // namespace zap
