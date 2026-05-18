#pragma once

#include <memory>

#include "gl/texture_pool_impl.h"
#include "gl/texture_pool_wrapper.h"
#include "log/log.h"
#include <thread>
#ifdef __APPLE__
#include "ios/gl/pixelbuffer_texture_pool.h"
#endif

namespace xhey::camera {

class TexturePool {
public:
    TexturePool() {}

    ~TexturePool() { XLOG_INFO(""); }

    std::shared_ptr<Texture> GetTexture(GLuint type, uint32_t width, uint32_t height, bool pixel_buffer_backed = true) {
        AssertInSameThread();
#ifdef __APPLE__
        if (type == GL_RGBA && pixel_buffer_backed) {
            // other types for ios are not tested.
            if (!pb_texture_pool_) {
                pb_texture_pool_ = std::make_shared<DynamicTexturePool<PixelBufferTexturePool>>();
            }

            auto ret = pb_texture_pool_->GetTexture(type, width, height);

            if (ret) {
                return ret;
            }
        }
#endif

        if (!normal_texture_pool_) {
            normal_texture_pool_ = std::make_shared<DynamicTexturePool<TexturePoolImpl>>();
        }

        auto texture = normal_texture_pool_->GetTexture(type, width, height);
        return texture;
    }

private:
    inline void AssertInSameThread() {
        if (thread_id_ == std::thread::id()) {
            thread_id_ = std::this_thread::get_id();
        }
        assert(thread_id_ == std::this_thread::get_id());
    }

private:
    std::shared_ptr<DynamicTexturePool<TexturePoolImpl>> normal_texture_pool_;
#ifdef __APPLE__
    std::shared_ptr<DynamicTexturePool<PixelBufferTexturePool>> pb_texture_pool_;
#endif
    std::thread::id thread_id_;
};

} // namespace xhey::camera
