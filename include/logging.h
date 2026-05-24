#pragma once

#include <string>
#include "http/status.h"

namespace zap {

enum class LogLevel { None, Error, Warning, Info, Debug };

class Logger {
public:
    explicit Logger(LogLevel level = LogLevel::Info);
    
    void request(const std::string& method, const std::string& path,
                 HttpStatus status, const std::string& note = "");
    void error(const std::string& method, const std::string& path,
               const std::string& error);
    void parameter_error(const std::string& method, const std::string& path,
                         size_t arg_index, const std::string& expected,
                         const std::string& got, const std::string& source_info);
    void exception(const std::string& method, const std::string& path,
                   const std::string& error, const std::string& source_info);
    
    void set_level(LogLevel level) { level_ = level; }
    
private:
    LogLevel level_;
    bool should_log(LogLevel msg_level) const;
};

} // namespace zap