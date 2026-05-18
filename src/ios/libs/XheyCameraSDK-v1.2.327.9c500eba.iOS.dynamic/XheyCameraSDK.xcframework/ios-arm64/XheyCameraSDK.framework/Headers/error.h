#pragma once

#include "common/error_types.h"
#include "common/misc_utils.h"
#include <sstream>
#include <string>

namespace xhey::camera {

#ifndef WIN32
#define MAKE_ERROR_CONTEXT                                                                                                       \
    xhey::camera::ErrorContext {                                                                                                 \
        __FILENAME__, __FUNCTION__, __LINE__                                                                                     \
    }
#else
#define MAKE_ERROR_CONTEXT                                                                                                       \
    xhey::camera::ErrorContext {                                                                                                 \
        __FILE__, __FUNCTION__, __LINE__                                                                                         \
    }
#endif
#define MAKE_ERROR(...) xhey::camera::Error(__VA_ARGS__).WithContext(MAKE_ERROR_CONTEXT)

struct ErrorContext {
    std::string file_name;
    std::string function_name;
    int line_num = 0;
};

class Error {
public:
    Error &WithContext(ErrorContext context) {
        context_ = context;
        return *this;
    }

    Error &WithType(ErrorType type) {
        error_type_ = type;
        return *this;
    }

    Error() : Error(kErrorCodeOK) {}

    Error(int code) : domain_(kErrorDomainDefault), code_(code) {}

    Error(ErrorDomain domain, int code) : domain_(domain), code_(code), error_type_(kErrorTypeMediaKit) {}

    Error(int code, const std::string &message) : domain_(kErrorDomainDefault), code_(code), message_(message) {}

    Error(ErrorDomain domain, int code, const std::string &message) : domain_(domain), code_(code), message_(message) {}

    static Error OK() { return Error(kErrorCodeOK); }

    int Code() const { return code_; }

    ErrorDomain Domain() const { return domain_; }

    const std::string &Message() const { return message_; }

    const ErrorType Type() const { return error_type_; }

    const ErrorContext &Context() const { return context_; }

    bool IsOK() const { return code_ == ErrorCode::kErrorCodeOK; }

    bool NotOK() const { return !IsOK(); }

    std::string Format() const {
        if (code_ == kErrorCodeOK) {
            return "Error: OK";
        }
        std::stringstream ss;
        ss << "Error:";
        if (!context_.file_name.empty()) {
            ss << context_.file_name << ":<" << context_.line_num << ">[" << context_.function_name << "]:";
        }
        ss << "domain:" << domain_;
        ss << "|code: " << code_;
        if (error_type_ != kErrorTypeMediaKit) {
            ss << "|type: " << error_type_;
        }
        if (!message_.empty()) {
            ss << "|message: " << message_;
        }
        return ss.str();
    }

private:
    ErrorDomain domain_ = kErrorDomainDefault;
    // the error code, if source is kErrorTypeMediaKit, then the code is from enum ErrorCode, otherwise, the code is
    // defined by 3rdparty
    int code_;
    ErrorType error_type_ = kErrorTypeMediaKit; // indicates where is this error from, from sdk or thirdparty system.
    std::string message_;
    ErrorContext context_;
};

} // namespace xhey::camera
