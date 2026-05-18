#pragma once

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <vector>
#include <zlib.h>

// 跨平台 gettid 实现
#ifdef __APPLE__
#include <pthread.h>
inline int gettid() {
    uint64_t tid;
    pthread_threadid_np(NULL, &tid);
    return static_cast<int>(tid);
}
#elif defined(__ANDROID__)

#include <sys/syscall.h>
#include <unistd.h>

inline int gettid() {
    return static_cast<int>(syscall(__NR_gettid));
}

#else
#include <unistd.h>
// Linux 等其他平台
inline int gettid() {
    return static_cast<int>(::gettid());
}
#endif

#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#ifdef __COUNTER__
#define UNIQUE_NAME(base) CONCATENATE(base, __COUNTER__)
#else
#define UNIQUE_NAME(base) CONCATENATE(base, __LINE__)
#endif

#if defined(_MSC_VER)
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define FORCE_INLINE inline __attribute__((always_inline))
#else
#define FORCE_INLINE inline
#endif

#define STRINGIFY_DETAIL(x) #x
#define STRINGIFY(x) STRINGIFY_DETAIL(x)

namespace xhey {
namespace camera {

class NonCopyable {
protected:
    NonCopyable() {}

    virtual ~NonCopyable() {}

private:
    NonCopyable(NonCopyable const &) = delete;

    NonCopyable &operator=(NonCopyable const &) = delete;

    NonCopyable(NonCopyable &&) = delete;

    NonCopyable &operator=(NonCopyable &&) = delete;
};

template <typename T>
class Singleton : public NonCopyable {
public:
    static T &SharedInstance() {
        static T *instance = new T();
        return *instance;
    }
};

class MiscUtils {
public:
    template <typename T>
    static inline T Align(T num, int align) {
        static_assert(std::numeric_limits<T>::is_integer);
        return ((num) + (align)-1) & ~((align)-1);
    }

    static std::vector<uint8_t> GzipDecompress(const std::vector<uint8_t> &data);

    static std::vector<uint8_t> GzipCompress(const std::vector<uint8_t> &data, int level = Z_BEST_COMPRESSION);
};

template <typename T>
struct ObjectWrapper {
    ObjectWrapper(const std::shared_ptr<T> &obj) : object(obj) {}

    ObjectWrapper() : object(nullptr) {}

    std::shared_ptr<T> object;
};

static constexpr const char *GetFileName(const char *path) {
    const char *file = path;
    for (const char *p = path; *p; ++p) {
        if (*p == '/')
            file = p + 1;
    }
    return file;
}

} // namespace camera
} // namespace xhey

#ifndef __FILENAME__
#define __FILENAME__ xhey::camera::GetFileName(__FILE__)
#endif
