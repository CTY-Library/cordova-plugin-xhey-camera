#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stdint.h>

namespace xhey::camera {

class Event {
public:
    Event();

    void Signal();
    void SignalMultiple(uint16_t count);
    void Wait();
    bool WaitFor(uint16_t msec);

private:
    bool signaled_;
    uint16_t signaled_count_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

class AvailableEvent {
public:
    AvailableEvent() {}
    AvailableEvent(bool available) : available_(available) {}

    ~AvailableEvent() = default;

    void Wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [&] { return available_; });
    }

    void PostAvailableSignal() {
        std::lock_guard<std::mutex> lock(mutex_);
        available_ = true;
        condition_.notify_one();
    }

    void PostUnavailableSignal() {
        std::lock_guard<std::mutex> lock(mutex_);
        available_ = false;
    }

private:
    bool available_ = true;
    std::mutex mutex_;
    std::condition_variable condition_;
};

} // namespace xhey::camera
