#pragma once

#include <mutex>

#include "common/object_pool.h"
#include "common/release_fence.h"
#include "common/stopwatch.h"
#include "gl/framebuffer.h"
#include "gl/texture_pool.h"
#include "gl_utils.h"
#include "log/log.h"
#include <thread>

namespace xhey::camera {

class FramebufferFactory : public ObjectPool<Framebuffer>::ObjectPoolFactory {
public:
    virtual Framebuffer *CreateNewObject() override { return new Framebuffer(); }
};

class FramebufferPool : protected ObjectPool<Framebuffer> {
public:
    FramebufferPool(TexturePool *texture_pool)
        : ObjectPool<Framebuffer>(std::make_shared<FramebufferFactory>(), 255), texture_pool_(texture_pool),
          fence_(new ReleaseFence()) {}

    virtual ~FramebufferPool() {
        XLOG_INFO("");
        auto lg = fence_->LockGuard();
        fence_->Released(true);
    }

    std::shared_ptr<Framebuffer> GetFramebuffer(int width, int height) {
        AssertInSameThread();
        // XLOG_INFO("width: %d, height: %d", width, height);
        CheckGlError("Before GetFramebuffer");
        Framebuffer *fbo = NewObjectRawPtr();
        CheckGlError("After NewObjectRawPtr");
        fbo->BindTexture(texture_pool_->GetTexture(GL_RGBA, width, height), width, height);
        CheckGlError("After BindTexture");
        auto fence = fence_;
        return std::shared_ptr<Framebuffer>(fbo, [fence, this](Framebuffer *fbo) {
            auto lg = fence->LockGuard();
            fbo->UnbindTexture();

            if (fence_->Released()) {
                delete fbo;
            } else {
                this->FreeObjectRawPtr(fbo);
            }
        });
    }

private:
    inline void AssertInSameThread() {
        if (thread_id_ == std::thread::id()) {
            thread_id_ = std::this_thread::get_id();
        }
        assert(thread_id_ == std::this_thread::get_id());
    }

private:
    TexturePool *texture_pool_;
    std::shared_ptr<ReleaseFence> fence_;
    std::thread::id thread_id_;
};

} // namespace xhey::camera
