#pragma once

#include <memory>

#include "gl.h"
#include "gl_locker.h"

namespace xhey::camera {

class Texture {
public:
    Texture(GLuint texture_id, bool is_oes = false);
    Texture(GLuint type, uint32_t width, uint32_t height);
    Texture(GLuint texture_id, const std::shared_ptr<void> &pixel_buffer);
    ~Texture();

    GLuint GetTextureId() const { return texture_id_; }
    bool IsOesTexture() const { return is_oes_; }

    std::shared_ptr<void> GetPixelBuffer() const { return pixel_buffer_; }

    GLLocker *ReaderLock() const { return reader_lock_.get(); }
    GLLocker *WriterLock() const { return writer_lock_.get(); }

private:
    GLuint texture_id_ = 0;
    bool is_oes_ = false;
    bool is_external_ = false;
    std::shared_ptr<void> pixel_buffer_;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    std::unique_ptr<GLLocker> reader_lock_;
    std::unique_ptr<GLLocker> writer_lock_;
};

} // namespace xhey::camera
