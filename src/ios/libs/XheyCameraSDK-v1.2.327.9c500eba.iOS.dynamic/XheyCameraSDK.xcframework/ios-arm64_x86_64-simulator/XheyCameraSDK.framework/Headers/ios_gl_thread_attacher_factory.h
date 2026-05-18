#pragma once

#include "gl/gl_thread_attacher.h"
#include <memory>

namespace xhey::camera {

class IosGlThreadAttacherFactory : public GlThreadAttacherFactory {
public:
    IosGlThreadAttacherFactory(EAGLSharegroup *sharegroup);
    ~IosGlThreadAttacherFactory();

    GlThreadAttacher *Create() override;

private:
    class Impl;

private:
    std::unique_ptr<Impl> impl_;
};
} // namespace xhey::camera
