#pragma once

#include <deque>
#include <mutex>

namespace xhey::camera {

template <class T>
class TaskQueue {
public:
    virtual ~TaskQueue() {}
    virtual bool PushBack(const T &val) = 0;
    virtual bool PopFront(T &ret) = 0;

    virtual bool PushFront(const T &val) = 0;
    virtual bool PopBack(T &ret) = 0;

    virtual size_t Size() = 0;
};


template <class T>
class TaskDeque : public TaskQueue<T> {
public:
    TaskDeque(size_t max_size) : max_size_(max_size) {}

    virtual ~TaskDeque() {}

    virtual bool PushBack(const T &val) override {
        std::unique_lock<std::mutex> lock(mutex_);
        if (deque_.size() >= max_size_) {
            return false;
        }
        deque_.push_back(val);
        return true;
    }

    virtual bool PopFront(T &ret) override {
        std::unique_lock<std::mutex> lock(mutex_);
        if (deque_.empty()) {
            return false;
        }
        ret = deque_.front();
        deque_.pop_front();
        return true;
    }

    virtual bool PushFront(const T &val) override {
        std::unique_lock<std::mutex> lock(mutex_);
        if (deque_.size() >= max_size_) {
            return false;
        }
        deque_.push_front(val);
        return true;
    }

    virtual bool PopBack(T &ret) override {
        std::unique_lock<std::mutex> lock(mutex_);
        if (deque_.empty()) {
            return false;
        }
        ret = deque_.back();
        deque_.pop_back();
        return true;
    }

    virtual size_t Size() override {
        std::unique_lock<std::mutex> lock(mutex_);
        return deque_.size();
    }

private:
    std::mutex mutex_;
    std::deque<T> deque_;
    size_t max_size_;
};

} // namespace xhey::camera
