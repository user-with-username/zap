#pragma once

#include <stdexcept>
#include <string>
#include <type_traits>

namespace zap {
template <typename T>
inline const char* type_name() {
    if (std::is_same_v<T, std::string>) return "std::string";
    if (std::is_same_v<T, int>)         return "int";
    if (std::is_same_v<T, long long>)   return "long long";
    if (std::is_same_v<T, double>)      return "double";
    if (std::is_same_v<T, float>)       return "float";
    if (std::is_same_v<T, bool>)        return "bool";
    return "unknown_type";
}

class ParameterCastException : public std::runtime_error {
public:
    ParameterCastException(const char* expected_type, size_t arg_index, const std::string& got_value)
        : std::runtime_error("Type conversion failed"),
          expected_type_(expected_type), arg_index_(arg_index), got_value_(got_value) {}

    const char*        expected_type() const { return expected_type_; }
    size_t             arg_index()     const { return arg_index_; }
    const std::string& got_value()     const { return got_value_; }

private:
    const char* expected_type_;
    size_t      arg_index_;
    std::string got_value_;
};

template <typename T>
typename std::enable_if_t<std::is_same_v<T, std::string>, T>
convert_param(const std::string& str, size_t) {
    return str;
}

template <typename T>
typename std::enable_if_t<std::is_same_v<T, bool>, T>
convert_param(const std::string& str, size_t arg_index) {
    if (str == "true"  || str == "1" || str == "yes") return true;
    if (str == "false" || str == "0" || str == "no")  return false;
    throw ParameterCastException(type_name<T>(), arg_index, str);
}

template <typename T>
typename std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, T>
convert_param(const std::string& str, size_t arg_index) {
    try {
        size_t   pos;
        long long val = std::stoll(str, &pos);
        if (pos != str.size()) throw std::invalid_argument("extra characters");
        return static_cast<T>(val);
    } catch (...) {
        throw ParameterCastException(type_name<T>(), arg_index, str);
    }
}

template <typename T>
typename std::enable_if_t<std::is_floating_point_v<T>, T>
convert_param(const std::string& str, size_t arg_index) {
    try {
        size_t pos;
        double val = std::stod(str, &pos);
        if (pos != str.size()) throw std::invalid_argument("extra characters");
        return static_cast<T>(val);
    } catch (...) {
        throw ParameterCastException(type_name<T>(), arg_index, str);
    }
}

} // namespace zap
