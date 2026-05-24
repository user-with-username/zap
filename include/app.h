#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <source_location>
#include <string>
#include <thread>
#include <vector>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "cors/cors_manager.h"
#include "http/request.h"
#include "http/response.h"
#include "logging.h"
#include "middleware.h"
#include "routing/group.h"
#include "routing/handler.h"
#include "routing/route_builder.h"
#include "routing/router.h"

namespace zap {

struct AppOptions {
    int thread_pool_size = 0;
    bool enable_cors = false;
    CorsConfig cors_config;
    bool enable_logging = true;
    LogLevel log_level = LogLevel::Info;
    
    AppOptions() {
        thread_pool_size = static_cast<int>(std::thread::hardware_concurrency());
        if (thread_pool_size == 0) thread_pool_size = 4;
    }
};

class App {
public:
    explicit App(const AppOptions& options = AppOptions());
    ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) noexcept = delete;
    App& operator=(App&&) noexcept = delete;

    App& threads(int n);
    App& enable_cors();
    App& disable_cors();
    App& allowed_origins(std::initializer_list<std::string> origins);
    App& allowed_methods(std::initializer_list<std::string> methods);
    App& allowed_headers(std::initializer_list<std::string> headers);
    App& allow_credentials(bool allow);
    App& set_log_level(LogLevel level);
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

    void listen(int port, const std::string& host = "0.0.0.0");
    void start(int port, const std::string& host = "0.0.0.0");
    void serve(int port, const std::string& host = "0.0.0.0");
    void stop();
    bool is_running() const;

    void add_route(const std::string& method, const std::string& path,
                   const std::vector<Middleware>& mws, Handler handler,
                   std::source_location loc = std::source_location::current());

    Router& router() { return router_; }
    const Router& router() const { return router_; }
    const std::vector<Middleware>& global_middlewares() const { return global_middlewares_; }

private:
    Router router_;
    CorsManager cors_manager_;
    Logger logger_;
    AppOptions options_;
    std::vector<Middleware> global_middlewares_;
    std::unique_ptr<httplib::Server> server_;
    std::thread server_thread_;
    std::atomic<bool> running_{false};

    void configure_server();
    void setup_routes();
    void handle_request(const httplib::Request& req, httplib::Response& res);
    void handle_cors_preflight(const httplib::Request& req, httplib::Response& res);
    void handle_no_route(const httplib::Request& req, httplib::Response& res, bool path_matched);
    void execute_handler(const httplib::Request& req, httplib::Response& res,
                         const Route& route, const std::vector<std::string>& params);
    std::vector<Middleware> build_middleware_pipeline(const Route& route) const;
    void run_pipeline(const std::vector<Middleware>& pipeline, Request& req,
                      std::function<void()> final_handler);
    void handle_http_exception(const httplib::Request& req, httplib::Response& res,
                               const HttpException& e);
    void handle_parameter_exception(const httplib::Request& req, httplib::Response& res,
                                    const ParameterCastException& e, const Handler& handler);
    void handle_std_exception(const httplib::Request& req, httplib::Response& res,
                              const std::exception& e, const Handler& handler);
};

App& get_app();

}