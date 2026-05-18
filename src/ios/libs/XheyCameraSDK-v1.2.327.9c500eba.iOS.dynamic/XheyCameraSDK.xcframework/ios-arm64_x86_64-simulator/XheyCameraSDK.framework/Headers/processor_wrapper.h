#pragma once

#include "common/egl_media_thread.h"
#include "processor.h"

namespace xhey::camera {

template <typename T>
class ProcessorWrapper {
public:
    ProcessorWrapper(const std::shared_ptr<Processor<T>> &processor, const std::shared_ptr<EglMediaThread> &egl_media_thread)
        : processor_(processor), egl_media_thread_(egl_media_thread) {
        assert(!processor_->IsWrapped());
        processor_->SetWrapped(true);
        processor_->SetEglMediaThread(egl_media_thread_);
    }

    ~ProcessorWrapper() = default;

    T Process(const T &input) {
        if (!processor_ || !processor_->IsEnable() || !input) {
            return input;
        }

        return processor_->Process(input);
    }

    std::shared_ptr<Processor<T>> GetProcessor() { return processor_; }

private:
    std::shared_ptr<Processor<T>> processor_;
    std::shared_ptr<EglMediaThread> egl_media_thread_;
};

template <>
inline std::shared_ptr<VideoFrame>
ProcessorWrapper<std::shared_ptr<VideoFrame>>::Process(const std::shared_ptr<VideoFrame> &input) {
    if (!processor_ || !processor_->IsEnable() || !input) {
        return input;
    }

    if (input->attributes.HasTransform() || input->type != VideoFrame::Type::kTexture ||
        (input->texture && input->texture->IsOesTexture())) {
        auto fbo = egl_media_thread_->GetRenderContext()->framebuffer_pool->GetFramebuffer(input->GetOutputWidth(),
                                                                                           input->GetOutputHeight());
        if (!fbo) {
            XLOG_ERROR("Get fbo failed.");
            return nullptr;
        }

        fbo->Bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        egl_media_thread_->GetRenderContext()->shader_program_pool->Draw(input.get(), true);

        std::shared_ptr<VideoFrame> processed_frame = std::make_shared<VideoFrame>();
        processed_frame->type = VideoFrame::Type::kTexture;
        processed_frame->texture = fbo->GetTexture();
        processed_frame->width = fbo->GetWidth();
        processed_frame->height = fbo->GetHeight();
        processed_frame->timestamp = input->timestamp;
        processed_frame->attributes = input->attributes;
        processed_frame->attributes.ClearTransform();
        return processor_->Process(processed_frame);
    }

    return processor_->Process(input);
}

} // namespace xhey::camera
