#pragma once

#include "gl/drawer/oes_drawer.h"
#include "gl/drawer/rgba_drawer.h"
#include "gl/drawer/texture_drawer.h"
#include "gl/drawer/yuv_drawer.h"
#include "gl/texture_reader.h"
#include <cassert>
#include <thread>

namespace xhey::camera {

class RenderContext;

class ShaderProgramPool {
public:
    ShaderProgramPool(RenderContext *render_context);

    ~ShaderProgramPool();

    void Draw(VideoFrame *video_frame, bool render_to_texture);

    void Draw(VideoFrame *video_frame, ViewPort view_port, bool render_to_texture);

    TextureReader *GetTextureReader();

private:
    OesDrawer *GetOesDrawer();

    RgbaDrawer *GetRgbaDrawer();

    TextureDrawer *GetTextureDrawer();

    YuvDrawer *GetYuvDrawer();

    inline void AssertInSameThread() {
        if (thread_id_ == std::thread::id()) {
            thread_id_ = std::this_thread::get_id();
        }
        assert(thread_id_ == std::this_thread::get_id());
    }

private:
    RenderContext *render_context_ = nullptr;
    std::unique_ptr<OesDrawer> oes_drawer_;
    std::unique_ptr<RgbaDrawer> rgba_drawer_;
    std::unique_ptr<TextureDrawer> texture_drawer_;
    std::unique_ptr<YuvDrawer> yuv_drawer_;
    std::unique_ptr<TextureReader> texture_reader_;
    std::thread::id thread_id_;
};

} // namespace xhey::camera
