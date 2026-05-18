#pragma once

#include <map>
#include <memory>
#include <tuple>

#include "gl/texture_pool_interface.h"

namespace xhey::camera {

template <class TexturePoolClass>
class DynamicTexturePool : public TexturePoolInterface {
public:
    typedef TexturePoolClass texture_pool_type_t;
    typedef std::tuple<GLuint, uint32_t, uint32_t> map_key_t;

    std::shared_ptr<Texture> GetTexture(GLuint type, uint32_t width, uint32_t height) override {
        auto tuple = std::make_tuple(type, width, height);

        if (map_.find(tuple) == map_.end()) {
            auto pool = std::make_shared<TexturePoolClass>(type, width, height);
            map_[tuple] = pool;
        }

        auto pool = map_[tuple];
        auto texture = pool->GetTexture(type, width, height);

        if (!texture) {
            map_.erase(tuple);
            return nullptr;
        }

        return texture;
    }

    virtual ~DynamicTexturePool() { XLOG_INFO(""); }

    void Clear() override { map_.clear(); }

private:
    std::map<map_key_t, std::shared_ptr<texture_pool_type_t>> map_;
};

} // namespace xhey::camera
