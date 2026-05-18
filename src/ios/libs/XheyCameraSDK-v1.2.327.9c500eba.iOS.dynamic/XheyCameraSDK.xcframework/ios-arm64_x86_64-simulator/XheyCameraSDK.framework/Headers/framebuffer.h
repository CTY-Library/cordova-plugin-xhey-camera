#pragma once

#include "texture.h"
#include <memory>

namespace xhey::camera {

class Framebuffer {
public:
    Framebuffer();
    ~Framebuffer();

    void BindTexture(const std::shared_ptr<Texture> &texture, int width, int height);
    void UnbindTexture();

    void Bind();
    void Unbind();

    void Clear();

    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }

    void Release();

    std::shared_ptr<Texture> GetTexture() const { return texture_; }

private:
    GLuint framebuffer_id_ = 0;
    std::shared_ptr<Texture> texture_;
    int width_ = 0;
    int height_ = 0;
};

} // namespace xhey::camera
