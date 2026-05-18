#pragma once

#include "gl/texture.h"
#include "gl/texture_pool_interface.h"
#include <CoreMedia/CoreMedia.h>
#include <CoreVideo/CoreVideo.h>
#include <VideoToolbox/VideoToolbox.h>
#include <map>
#include <memory>

namespace xhey {
namespace camera {

class PixelBufferTexturePool : public TexturePoolInterface {
public:
    PixelBufferTexturePool(GLuint type, uint32_t width, uint32_t height);
    virtual ~PixelBufferTexturePool();

    virtual std::shared_ptr<Texture> GetTexture(GLuint type, uint32_t width, uint32_t height) override;
    std::pair<uint32_t, uint32_t> resolution();

    virtual void Clear() override;

private:
    CVPixelBufferPoolRef pixel_buffer_pool_;

#if TARGET_OS_OSX
    CVOpenGLTextureCacheRef texture_cache_;
#else
    CVOpenGLESTextureCacheRef texture_cache_;
#endif
    std::shared_ptr<std::mutex> release_lock_;
    uint32_t width_;
    uint32_t height_;
    GLuint type_;
};
} // namespace camera
} // namespace xhey
