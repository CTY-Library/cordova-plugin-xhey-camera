#pragma once

#include <stdint.h>

#include <cassert>
#include <memory>
#include <mutex>
#include <vector>

#include "log/log.h"

namespace xhey::camera {

template <class T>
class ObjectPool : public std::enable_shared_from_this<ObjectPool<T>> {
public:
    static constexpr uint32_t kDefaultBlockSize = 32;
    static constexpr uint32_t kDefaultMaxObjectCount = 128;

    class ObjectPoolFactory {
    public:
        virtual T *CreateNewObject() = 0;
        virtual ~ObjectPoolFactory() {}
    };

    ObjectPool(std::shared_ptr<ObjectPoolFactory> factory, uint32_t max_size = ObjectPoolFactory::kDefaultMaxObjectCount)
        : free_object_count_(0), max_size_(max_size), factory_(factory) {
        free_objects_.resize(max_size);
    }

    virtual ~ObjectPool() {
        XLOG_INFO("");
        std::lock_guard<std::mutex> lg(mutex_);
        for (auto i = 0u; i < free_object_count_; ++i) {
            delete free_objects_[i];
            free_objects_[i] = nullptr;
        }
        free_object_count_ = 0;
    }

    std::shared_ptr<T> NewObject() {
        std::lock_guard<std::mutex> lg(mutex_);
        T *object = NewObjectInternal();

        std::weak_ptr<ObjectPool> weak_this = this->shared_from_this();
        return std::shared_ptr<T>(object, [weak_this](T *ptr) {
            std::shared_ptr<ObjectPool> thiz = weak_this.lock();
            if (thiz) {
                std::lock_guard<std::mutex> lg(thiz->mutex_);
                thiz->FreeObjectInternal(ptr);
            } else {
                // thiz already destroyed, just delete this ptr
                delete ptr;
            }
        });
    }

    T *NewObjectRawPtr() {
        std::lock_guard<std::mutex> lg(mutex_);
        return NewObjectInternal();
    }

    void FreeObjectRawPtr(T *ptr) {
        std::lock_guard<std::mutex> lg(mutex_);
        FreeObjectInternal(ptr);
    }

private:
    T *NewObjectInternal() {
        T *object = nullptr;
        if (free_object_count_ == 0) {
            object = factory_->CreateNewObject();
        } else {
            object = free_objects_[--free_object_count_];
        }
        return object;
    }

    void FreeObjectInternal(T *ptr) {
        if (free_object_count_ < max_size_) {
            free_objects_[free_object_count_++] = ptr;
        } else {
            // enough free objects, just delete this ptr
            delete ptr;
        }
    }

protected:
    uint32_t free_object_count_;
    std::vector<T *> free_objects_;
    std::mutex mutex_;

private:
    uint32_t max_size_;
    std::shared_ptr<ObjectPoolFactory> factory_;
};

} // namespace xhey::camera
