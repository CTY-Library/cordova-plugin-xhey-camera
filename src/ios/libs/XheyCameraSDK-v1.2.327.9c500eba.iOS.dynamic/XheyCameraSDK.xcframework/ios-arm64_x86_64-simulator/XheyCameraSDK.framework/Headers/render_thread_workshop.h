#pragma once

#include "common/connectable.h"
#include "common/egl_media_thread.h"
#include "common/media_types.h"
#include "frame_drop_manager.h"

namespace xhey::camera {

class RenderThreadWorkshop : public MediaWorkshop<std::shared_ptr<VideoFrame>>,
                             public std::enable_shared_from_this<RenderThreadWorkshop> {
public:
    RenderThreadWorkshop(const std::shared_ptr<EglMediaThread> &render_thread, const std::string &name);

    virtual ~RenderThreadWorkshop();

    void Release();

    void OnMediaArrived(const std::shared_ptr<VideoFrame> &video_frame, uint32_t source_id) override;

private:
    std::unique_ptr<AbstractFrameDropManager<VideoFrame>> frame_drop_manager_;
    std::shared_ptr<EglMediaThread> render_thread_;
    std::string name_;
};

} // namespace xhey::camera
