#pragma once

#ifdef __APPLE__
#include "ios/gl/pixelbuffer_texture_extractor.h"
#endif
#include "common/misc_utils.h"
#include "gl/framebuffer_pool.h"
#include "gl/shader_program_pool.h"
#include "gl/texture_loader.h"
#include "gl/texture_pool.h"
#include "log/log.h"

namespace xhey::camera {

class RenderContext final : public NonCopyable {
public:
#ifdef __APPLE__
    RenderContext()
        : texture_pool(std::make_unique<TexturePool>()), texture_loader(std::make_unique<TextureLoader>()),
          framebuffer_pool(std::make_unique<FramebufferPool>(texture_pool.get())),
          pixelbuffer_texture_extractor(std::make_unique<PixelbufferTextureExtractor>()) {
        XLOG_INFO("");
        shader_program_pool = new ShaderProgramPool(this);
    }
#else
    RenderContext()
        : texture_pool(std::make_unique<TexturePool>()), texture_loader(std::make_unique<TextureLoader>()),
          framebuffer_pool(std::make_unique<FramebufferPool>(texture_pool.get())) {
        XLOG_INFO("");
        shader_program_pool = new ShaderProgramPool(this);
    }
#endif

    ~RenderContext() {
        XLOG_INFO("");
        if (shader_program_pool) {
            delete shader_program_pool;
            shader_program_pool = nullptr;
        }
    }

    void ReclaimMemory() {
        texture_pool.reset(new TexturePool());
        texture_loader.reset(new TextureLoader());
        framebuffer_pool.reset(new FramebufferPool(texture_pool.get()));
#ifdef __APPLE__
        pixelbuffer_texture_extractor.reset(new PixelbufferTextureExtractor());
#endif
    }

    std::unique_ptr<TexturePool> texture_pool;
    std::unique_ptr<TextureLoader> texture_loader;
    std::unique_ptr<FramebufferPool> framebuffer_pool;
    ShaderProgramPool *shader_program_pool = nullptr;
#ifdef __APPLE__
    std::unique_ptr<PixelbufferTextureExtractor> pixelbuffer_texture_extractor;
#endif
};

} // namespace xhey::camera
