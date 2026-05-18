#pragma once

#include "common/misc_utils.h"
#include <functional>

namespace xhey::camera {

class Defer {
public:
    explicit Defer(const std::function<void()> &func) : func_(std::move(func)), active_(true) {}

    // 禁止拷贝构造和赋值
    Defer(const Defer &) = delete;
    Defer &operator=(const Defer &) = delete;

    // 移动构造和赋值
    Defer(Defer &&other) noexcept : func_(std::move(other.func_)), active_(other.active_) { other.active_ = false; }
    Defer &operator=(Defer &&other) noexcept {
        if (this != &other) {
            func_ = std::move(other.func_);
            active_ = other.active_;
            other.active_ = false;
        }
        return *this;
    }

    ~Defer() {
        if (active_) {
            func_();
        }
    }

    void cancel() { active_ = false; }

private:
    std::function<void()> func_;
    bool active_;
};

#define defer(code) auto UNIQUE_NAME(defer_) = Defer([&]() { code; })

} // namespace xhey::camera
