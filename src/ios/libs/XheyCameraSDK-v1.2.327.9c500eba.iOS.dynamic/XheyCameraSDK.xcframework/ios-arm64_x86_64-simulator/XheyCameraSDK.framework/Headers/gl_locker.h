#pragma once

#include "common/thread_affinity_guard.h"
#include "gl.h"
#include <mutex>
#include <vector>

namespace xhey {
namespace camera {

class GLLocker {
public:
    GLLocker();
    virtual ~GLLocker() {};
    // add fence and gpu command on other thread must wait for this fence.
    void AddFence();
    // blocked call until gpu command reached the fence.
    void Wait();
    // unblocked call to tell gpu following this sync.
    void Sync();
    // check whether or not the fence is signaled.
    bool Check();
    // reset the Fences.
    void Reset();

    void ForceEnableSync(bool enable) { enable_sync_ = enable; }

private:
    bool enable_sync_;
    std::vector<GLsync> gl_syncs;
    std::mutex lock_;
    ThreadAffinityGuard thread_affinity_guard_;
};

} // namespace camera
} // namespace xhey
