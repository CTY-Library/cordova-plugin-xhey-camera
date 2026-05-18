#pragma once

#include "abstract_frame_drop_manager.h"
#include "common/connectable.h"
#include "common/egl_media_thread.h"
#include "common/media_types.h"
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>

namespace xhey::camera {

class CpuThreadWorkshop : public MediaWorkshop<std::shared_ptr<VideoFrame>>,
                          public std::enable_shared_from_this<CpuThreadWorkshop> {
public:
    CpuThreadWorkshop(const std::shared_ptr<EglMediaThread> &egl_media_thread);
    virtual ~CpuThreadWorkshop();

    void Release();

protected:
    void OnMediaArrived(const std::shared_ptr<VideoFrame> &frame, uint32_t source_id);

private:
    std::unique_ptr<AbstractFrameDropManager<VideoFrame>> frame_drop_manager_;
    std::shared_ptr<EglMediaThread> cpu_thread_;
};

} // namespace xhey::camera
