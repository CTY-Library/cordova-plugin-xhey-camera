#pragma once
#include "common/media_types.h"
#include <tuple>

namespace xhey {
namespace camera {

namespace internal {
class PixelbufferTextureExtractorImpl;
}

class PixelbufferTextureExtractor {
public:
    PixelbufferTextureExtractor();
    virtual ~PixelbufferTextureExtractor();

    bool ExtractPixelbuffer(void *pixel_buffer, std::shared_ptr<Texture> *y, std::shared_ptr<Texture> *u,
                            std::shared_ptr<Texture> *v, std::shared_ptr<Texture> *uv, std::shared_ptr<Texture> *rgba);

private:
    internal::PixelbufferTextureExtractorImpl *impl_;
};
} // namespace camera
} // namespace xhey
