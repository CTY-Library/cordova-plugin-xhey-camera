#pragma once

#include "common/media_types.h"

namespace xhey::camera {

template <class MediaType>
class AbstractFrameDropManager {
public:
    AbstractFrameDropManager() {}
    virtual ~AbstractFrameDropManager() {}
    virtual void
    SetFrameDropCallback(const std::function<void(const std::shared_ptr<MediaType> &, bool from_input)> &callback) = 0;
    virtual bool InputVideoFrame(const std::shared_ptr<MediaType> &) = 0;
    virtual std::shared_ptr<MediaType> OutputVideoFrame() = 0;
    virtual void DrainFrameUntilSize(uint32_t size) = 0;
    virtual void SetDropAllEnabled(bool enabled) = 0;
    virtual void SetFrameCountChangedCallback(const std::function<void(int64_t)> &callback) = 0;
    virtual void Clear() = 0;
};

} // namespace xhey::camera
