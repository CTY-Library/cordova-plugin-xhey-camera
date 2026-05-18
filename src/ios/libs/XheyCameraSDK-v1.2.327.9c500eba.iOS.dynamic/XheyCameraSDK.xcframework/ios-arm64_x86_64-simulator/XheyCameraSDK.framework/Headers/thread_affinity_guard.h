#pragma once

#include <cassert>
#include <thread>

namespace xhey::camera {
class ThreadAffinityGuard {
public:
    ThreadAffinityGuard() : thread_id_(std::this_thread::get_id()) {}

    inline void AssertInSameThread() { assert(thread_id_ == std::this_thread::get_id()); }

    bool IsInSameThread() { return thread_id_ == std::this_thread::get_id(); }

private:
    std::thread::id thread_id_;
};
} // namespace xhey::camera
