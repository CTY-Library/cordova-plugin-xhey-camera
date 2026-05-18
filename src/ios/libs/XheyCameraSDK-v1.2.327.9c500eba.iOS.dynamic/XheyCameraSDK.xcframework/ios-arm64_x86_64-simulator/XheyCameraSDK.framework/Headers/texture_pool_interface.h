#pragma once

#include "gl/texture.h"
#include "log/log.h"

namespace xhey::camera {

class TexturePoolInterface {
public:
    virtual ~TexturePoolInterface() { XLOG_INFO(""); }

    virtual std::shared_ptr<Texture> GetTexture(GLuint type, uint32_t width, uint32_t height) = 0;

    virtual void Clear() = 0;
};

} // namespace xhey::camera
