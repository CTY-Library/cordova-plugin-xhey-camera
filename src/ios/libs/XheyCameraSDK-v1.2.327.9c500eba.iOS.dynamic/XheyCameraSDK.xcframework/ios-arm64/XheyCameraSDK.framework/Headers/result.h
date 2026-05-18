#pragma once

#include "error.h"
#include "optional.h"

namespace xhey::camera {

class VoidType {};

template <typename T>
class Result {
public:
    Result(T value) : value_(value), success_(true) {}
    Result(Error e) : error_(e), success_(false) {}

    T &Value() { return value_.Value(); }
    const T &Value() const { return value_.Value(); }
    const class Error &Error() const { return error_; }
    class Error &Error() { return error_; }

    bool Success() { return success_; }
    bool Success() const { return success_; }

private:
    Optional<T> value_;
    class Error error_;
    bool success_;
};

} // namespace xhey::camera
