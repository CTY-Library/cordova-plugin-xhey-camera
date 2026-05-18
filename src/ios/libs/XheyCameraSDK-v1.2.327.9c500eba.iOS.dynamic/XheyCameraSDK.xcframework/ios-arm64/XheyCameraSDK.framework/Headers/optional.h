#pragma once

#include <utility>

namespace xhey::camera {

template <class T>
class Optional {
public:
    static Optional<T> Null() { return Optional<T>(); }

    Optional() : has_value_(false) {}

    Optional(const T &t) : t_(t), has_value_(true) {}

    Optional(T &&t) : t_(std::move(t)), has_value_(true) {}

    bool HasValue() { return has_value_; }

    bool HasValue() const { return has_value_; }

    T &Value() {
        if (has_value_) {
            return t_;
        } else {
            static T t;
            return t;
        }
    }

    const T &Value() const {
        if (has_value_) {
            return t_;
        } else {
            static T t;
            return t;
        }
    }

    void Reset() {
        has_value_ = false;
        t_ = T();
    }

    void Reset(const T &t) {
        has_value_ = true;
        t_ = t;
    }

private:
    T t_;
    bool has_value_ = false;
};

} // namespace xhey::camera
