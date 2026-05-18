#pragma once

#include <cstdint>
#if defined(__APPLE__)
#include <mach/mach_time.h>
#elif defined(__ANDROID__)
#include <time.h>
#endif
#include <string>

namespace xhey::camera {

class TimeUtils {
public:
    static std::string FormatTime(int64_t milliseconds, const std::string &format = "yyyy-MM-dd HH:mm:ss.SSS");

    static std::string FormatDurationToHumanReadable(int64_t milliseconds);

#if defined(__APPLE__)
#include <mach/mach_time.h>
    static inline int64_t GetBootTime() {
        // mach_absolute_time 返回纳秒级别计时，iOS/macOS 不包含休眠时间
        static mach_timebase_info_data_t timebase;
        if (timebase.denom == 0) {
            mach_timebase_info(&timebase);
        }

        uint64_t t = mach_absolute_time();
        // 转换为纳秒，再转毫秒
        t = t * timebase.numer / timebase.denom;
        return static_cast<int64_t>(t / 1000000); // 毫秒
    }

#elif defined(__ANDROID__)
#include <time.h>

    static inline int64_t GetBootTime() {
#if __ANDROID_API__ >= 17
        struct timespec ts;
        if (clock_gettime(CLOCK_BOOTTIME, &ts) != 0) {
            return -1; // 错误返回
        }
        // 纳秒转毫秒
        return static_cast<int64_t>(ts.tv_sec) * 1000LL + ts.tv_nsec / 1000000LL;
#else
        // API <17 用 CLOCK_MONOTONIC，无法包含休眠时间
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
            return -1;
        }
        return static_cast<int64_t>(ts.tv_sec) * 1000LL + ts.tv_nsec / 1000000LL;
#endif
    }

#else
#error Unsupported platform
#endif
};

} // namespace xhey::camera
