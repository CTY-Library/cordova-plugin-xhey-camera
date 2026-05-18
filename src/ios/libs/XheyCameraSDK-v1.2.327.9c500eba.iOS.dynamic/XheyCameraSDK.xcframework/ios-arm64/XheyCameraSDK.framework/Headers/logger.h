#pragma once

#include <cstdarg>
#include <cstring>
#include <memory>

#include "common/misc_utils.h"

namespace xhey {
namespace camera {

class ILogger {
public:
    virtual ~ILogger() = default;

    enum class LogLevel {
        kDebug,
        kInfo,
        kWarn,
        kError,
    };

    virtual void Log(LogLevel level, const char *fmt, ...) = 0;
    virtual void Log(LogLevel level, const char *fmt, va_list args) = 0;
};

class Logger : public Singleton<Logger> {
public:
    Logger();
    ~Logger();

    void Log(ILogger::LogLevel level, const char *fmt, ...);
    void Log(ILogger::LogLevel level, const char *fmt, va_list args);

    void SetLogger(std::unique_ptr<ILogger> logger);

private:
    std::unique_ptr<ILogger> _logger;
};

} // namespace camera
} // namespace xhey
