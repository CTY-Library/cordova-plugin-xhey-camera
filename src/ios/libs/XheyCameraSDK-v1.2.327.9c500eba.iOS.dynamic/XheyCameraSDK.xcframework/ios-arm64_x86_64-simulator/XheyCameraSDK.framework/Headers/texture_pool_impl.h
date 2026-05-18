#pragma once

#include "common/object_pool.h"
#include "gl/texture_pool_interface.h"

namespace xhey::camera {

class TextureFactory : public ObjectPool<Texture>::ObjectPoolFactory {
public:
    TextureFactory(GLuint type, uint32_t width, uint32_t height) : type_(type), width_(width), height_(height) {}
    ~TextureFactory() { XLOG_INFO(""); }

    virtual Texture *CreateNewObject() override { return new Texture(type_, width_, height_); }

private:
    GLuint type_;
    uint32_t width_;
    uint32_t height_;
};

class TexturePoolImpl : public TexturePoolInterface, public ObjectPool<Texture> {
public:
    TexturePoolImpl(GLuint type, uint32_t width, uint32_t height)
        : ObjectPool<Texture>(std::make_shared<TextureFactory>(type, width, height), 255), type_(type), width_(width),
          height_(height) {}

    virtual std::shared_ptr<Texture> GetTexture(GLuint type, uint32_t width, uint32_t height) override {
        assert(type_ == type && width_ == width && height_ == height);
        auto texture = NewObject();

        return texture;
    }

    virtual ~TexturePoolImpl() { XLOG_INFO(""); }

    std::shared_ptr<Texture> NewObject() {
        std::vector<std::shared_ptr<Texture>> tmp;
        std::shared_ptr<Texture> texture = ObjectPool<Texture>::NewObject();
        return texture;
    }

    void Clear() override {}

private:
    GLuint type_;
    uint32_t width_;
    uint32_t height_;
};

} // namespace xhey::camera
