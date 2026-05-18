#pragma once

#include "common/misc_utils.h"
#include "logger.h"

namespace xhey {
namespace camera {

#define XLOG_DEBUG(fmt, ...)                                                                                                     \
    xhey::camera::Logger::SharedInstance().Log(xhey::camera::ILogger::LogLevel::kDebug, ("[%d][%s][%d][%s]: " fmt), gettid(),    \
                                               __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define XLOG_INFO(fmt, ...)                                                                                                      \
    xhey::camera::Logger::SharedInstance().Log(xhey::camera::ILogger::LogLevel::kInfo, ("[%d][%s][%d][%s]: " fmt), gettid(),     \
                                               __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define XLOG_INFO_EVERY(n, fmt, ...)                                                                                             \
    do {                                                                                                                         \
        static uint32_t __cnt = 0;                                                                                               \
        if ((__cnt % (n)) == 0) {                                                                                                \
            xhey::camera::Logger::SharedInstance().Log(xhey::camera::ILogger::LogLevel::kInfo, ("[%d][%s][%d][%s]: " fmt),       \
                                                       gettid(), __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__);           \
        }                                                                                                                        \
        ++__cnt;                                                                                                                 \
    } while (0)

#define XLOG_WARN(fmt, ...)                                                                                                      \
    xhey::camera::Logger::SharedInstance().Log(xhey::camera::ILogger::LogLevel::kWarn, ("[%d][%s][%d][%s]: " fmt), gettid(),     \
                                               __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define XLOG_ERROR(fmt, ...)                                                                                                     \
    xhey::camera::Logger::SharedInstance().Log(xhey::camera::ILogger::LogLevel::kError, ("[%d][%s][%d][%s]: " fmt), gettid(),    \
                                               __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define XLOG_FATAL(fmt, ...)                                                                                                     \
    xhey::camera::Logger::SharedInstance().Log(xhey::camera::ILogger::LogLevel::kFatal, ("[%d][%s][%d][%s]: " fmt), gettid(),    \
                                               __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

} // namespace camera
} // namespace xhey
