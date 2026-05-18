#pragma once

#include "common/runloop.h"
#include "defs.h"
#include "gl/gl_thread_attacher.h"
#include "gl/shader_program_pool.h"
#include "render_context.h"
#include <string>

namespace xhey::camera {

class EglMediaThread : public std::enable_shared_from_this<EglMediaThread> {
public:
    EglMediaThread(const std::string &name);
    virtual ~EglMediaThread();

    void Init(const std::shared_ptr<GlThreadAttacherFactory> &factory);

    void Post(const std::function<void()> &task);

    bool MakeCurrent();

    bool AttachIfNeeded();

    std::shared_ptr<RenderContext> GetRenderContext() { return render_context_; }

    std::thread::id ThreadId() { return runloop_->thread_id(); }

    int64_t AddTimer(const std::function<void()> &func, uint32_t interval, bool repeat);
    void RemoveTimer(int64_t tid);

private:
    std::shared_ptr<GlThreadAttacher> gl_thread_attacher_;
    std::shared_ptr<Runloop> runloop_;
    std::shared_ptr<RenderContext> render_context_;
    std::string name_;
};

} // namespace xhey::camera
