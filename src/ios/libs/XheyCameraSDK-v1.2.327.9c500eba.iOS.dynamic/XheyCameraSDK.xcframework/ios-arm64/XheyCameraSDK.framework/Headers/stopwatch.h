#pragma once

#include "../log/log.h"
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <string>

namespace xhey::camera {
// Stopwatch 类
class Stopwatch {
public:
    explicit Stopwatch(const std::string &tag) : tag_(tag), start_time_(std::chrono::steady_clock::now()) {}

    ~Stopwatch() { Stop(); }

    void Stop() {
        if (!valid_) {
            return;
        }
        valid_ = false;

        auto end_time = std::chrono::steady_clock::now();
        auto duration = end_time - start_time_;

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(duration - ms);
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - ms - us);

        // 这里用你自己的日志宏 XLOG_INFO，注意%lld需要配对应类型，可能用%ld 或 %lld 视平台而定
        XLOG_INFO("StopWatch: %s, duration: %lld ms %lld us %lld ns", tag_.c_str(), static_cast<long long>(ms.count()),
                  static_cast<long long>(us.count()), static_cast<long long>(ns.count()));
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
    std::string tag_;
    bool valid_ = true;
};

// 格式化字符串工具函数（类似printf，返回std::string）
inline std::string StringFormat(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buf[256];
    int n = vsnprintf(buf, sizeof(buf), fmt, args);

    va_end(args);

    if (n < 0) {
        return "";
    }
    if (n < (int)sizeof(buf)) {
        return std::string(buf, n);
    }

    // 超过256长度时，重新分配空间
    std::string ret(n + 1, '\0');
    va_start(args, fmt);
    vsnprintf(&ret[0], ret.size(), fmt, args);
    va_end(args);
    ret.resize(n);
    return ret;
}
} // namespace xhey::camera

// 辅助宏：拼接唯一变量名，防止重复
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

// 你用的 STRINGIFY 宏，若没定义，补一个
#ifndef STRINGIFY
#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)
#endif

// 默认的tag格式：[文件名][函数名][行号]
#define DEFAULT_STOPWATCH_TAG()                                                                                                  \
    ("[" + std::string(__FILENAME__) + "][" + std::string(__FUNCTION__) + "][" + STRINGIFY(__LINE__) + "]")

// 辅助宏：参数选择器，宏重载关键
#define GET_3RD_ARG(arg1, arg2, arg3, ...) arg3
#define AddStopwatchSelect(...) GET_3RD_ARG(__VA_ARGS__, AddStopwatchWithFormat, AddStopwatchDefault)

// 无参数调用，默认tag
#define AddStopwatchDefault() xhey::camera::Stopwatch UNIQUE_NAME(stopwatch_)(DEFAULT_STOPWATCH_TAG())

// 有参数调用，第一个参数是格式字符串，后面是格式化参数
#define AddStopwatchWithFormat(fmt, ...)                                                                                         \
    Stopwatch UNIQUE_NAME(stopwatch_)(DEFAULT_STOPWATCH_TAG() + " " + xhey::camera::StringFormat(fmt, ##__VA_ARGS__))

// 主入口宏，兼容无参和有参（至少一个参数）
#define AddStopwatch(...) AddStopwatchSelect(__VA_ARGS__, AddStopwatchWithFormat, AddStopwatchDefault)(__VA_ARGS__)
