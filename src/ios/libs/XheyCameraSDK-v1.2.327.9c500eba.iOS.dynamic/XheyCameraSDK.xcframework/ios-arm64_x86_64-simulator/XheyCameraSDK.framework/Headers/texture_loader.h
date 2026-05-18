#pragma once

#include "gl/gl_utils.h"

namespace xhey::camera {

class TextureLoader {
public:
    TextureLoader() {
#if defined(__ANDROID__)
        es3_supported_ = IsEs3Supported();
        XLOG_INFO("es3_support: %d", es3_supported_);
#else
        es3_supported_ = false;
#endif
    }

    template <GLuint type>
    void LoadData2Texture2D(int width, int height, uint8_t *raw_data, int stride) {
        uint8_t *data_to_load = nullptr;
        int old_alignment;

        if (es3_supported_) {
#if defined(__ANDROID__)

            if (type == GL_LUMINANCE) {
                glGetIntegerv(GL_UNPACK_ALIGNMENT, &old_alignment);

                if (stride % 4 == 0) {
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
                } else if (stride % 2 == 0) {
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
                } else {
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                }

                glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
            } else if (type == GL_RGBA) {
                glPixelStorei(GL_UNPACK_ROW_LENGTH, stride / 4);
            } else if (type == GL_LUMINANCE_ALPHA) {
                glPixelStorei(GL_UNPACK_ROW_LENGTH, stride / 2);
            }

            data_to_load = raw_data;
#else
            assert(0);
#endif
        } else {
            glGetIntegerv(GL_UNPACK_ALIGNMENT, &old_alignment);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            size_t copy_size = width;

            if (type == GL_LUMINANCE) {
                // align to 4 bytes
                if (width % 4 != 0) {
                    copy_size = (width / 4 + 1) * 4;
                }
            } else if (type == GL_RGBA) {
                copy_size = width * 4;
            } else if (type == GL_LUMINANCE_ALPHA) {
                copy_size = width * 2;

                if (width % 2 != 0) {
                    copy_size = (width / 2 + 1) * 4;
                }
            }

            if (copy_size == stride) {
                data_to_load = raw_data;
            } else {
                if (!buffer_ || buffer_len_ < copy_size * height) {
                    buffer_.reset(new uint8_t[copy_size * height]);
                    buffer_len_ = copy_size * height;
                }

                uint8_t *p = buffer_.get();
                uint8_t *q = raw_data;

                for (int i = 0; i < height; ++i) {
                    memcpy(p, q, copy_size);
                    p += copy_size;
                    q += stride;
                }

                data_to_load = buffer_.get();
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data_to_load);

        if (es3_supported_) {
#if defined(__ANDROID__)
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

            if (type == GL_LUMINANCE) {
                glPixelStorei(GL_UNPACK_ALIGNMENT, old_alignment);
            }

#else
            assert(0);
#endif
        } else {
            glPixelStorei(GL_UNPACK_ALIGNMENT, old_alignment);
        }
    }

    template <GLuint type> // type is GL_LUMINANCE or GL_RGBA
    GLenum LoadDataToTexture(GLuint tex_id, int width, int height, uint8_t *raw_data, int stride) {
        glBindTexture(GL_TEXTURE_2D, tex_id);
        CheckGlError("before LoadData2Texture2D.");
        LoadData2Texture2D<type>(width, height, raw_data, stride);
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGlError("Bind Textures.");
        return GL_NO_ERROR;
    }

private:
    bool es3_supported_;
    std::unique_ptr<uint8_t[]> buffer_;
    size_t buffer_len_ = 0;
};


} // namespace xhey::camera
