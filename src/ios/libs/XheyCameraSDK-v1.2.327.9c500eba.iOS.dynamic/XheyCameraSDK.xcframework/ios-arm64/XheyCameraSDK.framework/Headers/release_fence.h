#pragma once

namespace xhey::camera {

class ReleaseFence {
public:
    class Lock {
    public:
        Lock(std::shared_ptr<std::recursive_mutex> lock) : lock_(lock) { lock_->lock(); }
        virtual ~Lock() { lock_->unlock(); }

    private:
        std::shared_ptr<std::recursive_mutex> lock_;
    };

    ReleaseFence() : lock_(new std::recursive_mutex) {}

    bool Released() { return released_; }

    bool Released(bool mark) {
        auto lg = LockGuard();
        released_ = mark;
        return released_;
    }

    std::shared_ptr<Lock> LockGuard() { return std::make_shared<Lock>(lock_); }

private:
    volatile bool released_ = false;
    std::shared_ptr<std::recursive_mutex> lock_;
};

inline void Assert(bool condition) {
#ifdef DEBUG
    assert(condition);
#endif
}

} // namespace xhey::camera
