#pragma once

namespace zap {
enum class HttpStatus {
    // 1xx — Informational
    Continue                      = 100,
    SwitchingProtocols            = 101,
    Processing                    = 102,
    EarlyHints                    = 103,

    // 2xx — Success
    OK                            = 200,
    Created                       = 201,
    Accepted                      = 202,
    NonAuthoritativeInformation   = 203,
    NoContent                     = 204,
    ResetContent                  = 205,
    PartialContent                = 206,
    MultiStatus                   = 207,
    AlreadyReported               = 208,
    IMUsed                        = 226,

    // 3xx — Redirection
    MultipleChoices               = 300,
    MovedPermanently              = 301,
    Found                         = 302,
    SeeOther                      = 303,
    NotModified                   = 304,
    UseProxy                      = 305,
    TemporaryRedirect             = 307,
    PermanentRedirect             = 308,

    // 4xx — Client Error
    BadRequest                    = 400,
    Unauthorized                  = 401,
    PaymentRequired               = 402,
    Forbidden                     = 403,
    NotFound                      = 404,
    MethodNotAllowed              = 405,
    NotAcceptable                 = 406,
    ProxyAuthenticationRequired   = 407,
    RequestTimeout                = 408,
    Conflict                      = 409,
    Gone                          = 410,
    LengthRequired                = 411,
    PreconditionFailed            = 412,
    PayloadTooLarge               = 413,
    URITooLong                    = 414,
    UnsupportedMediaType          = 415,
    RangeNotSatisfiable           = 416,
    ExpectationFailed             = 417,
    ImATeapot                     = 418,
    MisdirectedRequest            = 421,
    UnprocessableEntity           = 422,
    Locked                        = 423,
    FailedDependency              = 424,
    TooEarly                      = 425,
    UpgradeRequired               = 426,
    PreconditionRequired          = 428,
    TooManyRequests               = 429,
    RequestHeaderFieldsTooLarge   = 431,
    UnavailableForLegalReasons    = 451,

    // 5xx — Server Error
    InternalServerError           = 500,
    NotImplemented                = 501,
    BadGateway                    = 502,
    ServiceUnavailable            = 503,
    GatewayTimeout                = 504,
    HTTPVersionNotSupported       = 505,
    VariantAlsoNegotiates         = 506,
    InsufficientStorage           = 507,
    LoopDetected                  = 508,
    NotExtended                   = 510,
    NetworkAuthenticationRequired = 511,
};

inline int to_code(HttpStatus status) {
    return static_cast<int>(status);
}

inline const char* to_string(HttpStatus status) {
    switch (status) {
        case HttpStatus::Continue:                      return "Continue";
        case HttpStatus::SwitchingProtocols:            return "Switching Protocols";
        case HttpStatus::Processing:                    return "Processing";
        case HttpStatus::EarlyHints:                    return "Early Hints";
        case HttpStatus::OK:                            return "OK";
        case HttpStatus::Created:                       return "Created";
        case HttpStatus::Accepted:                      return "Accepted";
        case HttpStatus::NonAuthoritativeInformation:   return "Non-Authoritative Information";
        case HttpStatus::NoContent:                     return "No Content";
        case HttpStatus::ResetContent:                  return "Reset Content";
        case HttpStatus::PartialContent:                return "Partial Content";
        case HttpStatus::MultiStatus:                   return "Multi-Status";
        case HttpStatus::AlreadyReported:               return "Already Reported";
        case HttpStatus::IMUsed:                        return "IM Used";
        case HttpStatus::MultipleChoices:               return "Multiple Choices";
        case HttpStatus::MovedPermanently:              return "Moved Permanently";
        case HttpStatus::Found:                         return "Found";
        case HttpStatus::SeeOther:                      return "See Other";
        case HttpStatus::NotModified:                   return "Not Modified";
        case HttpStatus::UseProxy:                      return "Use Proxy";
        case HttpStatus::TemporaryRedirect:             return "Temporary Redirect";
        case HttpStatus::PermanentRedirect:             return "Permanent Redirect";
        case HttpStatus::BadRequest:                    return "Bad Request";
        case HttpStatus::Unauthorized:                  return "Unauthorized";
        case HttpStatus::PaymentRequired:               return "Payment Required";
        case HttpStatus::Forbidden:                     return "Forbidden";
        case HttpStatus::NotFound:                      return "Not Found";
        case HttpStatus::MethodNotAllowed:              return "Method Not Allowed";
        case HttpStatus::NotAcceptable:                 return "Not Acceptable";
        case HttpStatus::ProxyAuthenticationRequired:   return "Proxy Authentication Required";
        case HttpStatus::RequestTimeout:                return "Request Timeout";
        case HttpStatus::Conflict:                      return "Conflict";
        case HttpStatus::Gone:                          return "Gone";
        case HttpStatus::LengthRequired:                return "Length Required";
        case HttpStatus::PreconditionFailed:            return "Precondition Failed";
        case HttpStatus::PayloadTooLarge:               return "Payload Too Large";
        case HttpStatus::URITooLong:                    return "URI Too Long";
        case HttpStatus::UnsupportedMediaType:          return "Unsupported Media Type";
        case HttpStatus::RangeNotSatisfiable:           return "Range Not Satisfiable";
        case HttpStatus::ExpectationFailed:             return "Expectation Failed";
        case HttpStatus::ImATeapot:                     return "I'm a teapot";
        case HttpStatus::MisdirectedRequest:            return "Misdirected Request";
        case HttpStatus::UnprocessableEntity:           return "Unprocessable Entity";
        case HttpStatus::Locked:                        return "Locked";
        case HttpStatus::FailedDependency:              return "Failed Dependency";
        case HttpStatus::TooEarly:                      return "Too Early";
        case HttpStatus::UpgradeRequired:               return "Upgrade Required";
        case HttpStatus::PreconditionRequired:          return "Precondition Required";
        case HttpStatus::TooManyRequests:               return "Too Many Requests";
        case HttpStatus::RequestHeaderFieldsTooLarge:   return "Request Header Fields Too Large";
        case HttpStatus::UnavailableForLegalReasons:    return "Unavailable For Legal Reasons";
        case HttpStatus::InternalServerError:           return "Internal Server Error";
        case HttpStatus::NotImplemented:                return "Not Implemented";
        case HttpStatus::BadGateway:                    return "Bad Gateway";
        case HttpStatus::ServiceUnavailable:            return "Service Unavailable";
        case HttpStatus::GatewayTimeout:                return "Gateway Timeout";
        case HttpStatus::HTTPVersionNotSupported:       return "HTTP Version Not Supported";
        case HttpStatus::VariantAlsoNegotiates:         return "Variant Also Negotiates";
        case HttpStatus::InsufficientStorage:           return "Insufficient Storage";
        case HttpStatus::LoopDetected:                  return "Loop Detected";
        case HttpStatus::NotExtended:                   return "Not Extended";
        case HttpStatus::NetworkAuthenticationRequired: return "Network Authentication Required";
        default:                                        return "Unknown Status";
    }
}

inline bool is_success(HttpStatus status) {
    const int code = to_code(status);
    return code >= 200 && code < 300;
}

} // namespace zap
