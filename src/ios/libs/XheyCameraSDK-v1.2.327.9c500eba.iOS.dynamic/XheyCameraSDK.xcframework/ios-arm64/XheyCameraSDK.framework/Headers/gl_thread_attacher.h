#pragma once

#ifdef ANDROID
#include <jni.h>
#include <stdint.h>
#endif

#include "gl.h"

namespace xhey::camera {

class GlThreadAttacher {
public:
    virtual ~GlThreadAttacher() {}

    virtual bool MakeCurrent() = 0;

    virtual bool AttachIfNeeded() = 0;

#ifdef ANDROID
    virtual bool UpdateSurface(jobject surface) = 0;

    virtual void SwapBuffers() = 0;

    virtual void SwapBuffers(uint64_t ts) = 0;
#endif

#ifdef OHOS_PLATFORM
    virtual bool UpdateNativeWindow(EGLNativeWindowType window) = 0;
    virtual bool SwapBuffers() = 0;
    virtual bool SwapBuffers(uint64_t timestamp_in_ns) = 0;
#endif

    virtual void Detach() = 0;

protected:
    virtual bool Attach() = 0;
    bool attached_;
};

class GlThreadAttacherFactory {
public:
    virtual ~GlThreadAttacherFactory() {}

    virtual GlThreadAttacher *Create() = 0;

#ifdef ANDROID
    virtual GlThreadAttacher *Create(jobject surface) = 0;

    virtual GlThreadAttacher *Create(jobject surface, bool recordable) = 0;
#endif
};

} // namespace xhey::camera
