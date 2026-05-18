#pragma once

#include "common/misc_utils.h"
#include "gl.h"
#include <mutex>
#include <unordered_map>

namespace xhey::camera {

class GlResourceMonitor : public Singleton<GlResourceMonitor> {
public:
    void AddTexture(GLuint texture_id);
    void RemoveTexture(GLuint texture_id);
    void AddProgram(GLuint program_id);
    void RemoveProgram(GLuint program_id);
    void AddFrameBuffer(GLuint frame_buffer_id);
    void RemoveFrameBuffer(GLuint frame_buffer_id);

    void CheckResourceLeak();

private:
    friend class Singleton<GlResourceMonitor>;

private:
    GlResourceMonitor();
    ~GlResourceMonitor();

    std::mutex mutex_;
    std::unordered_map<GLuint, int> texture_count_;
    std::unordered_map<GLuint, int> program_count_;
    std::unordered_map<GLuint, int> frame_buffer_count_;
};

} // namespace xhey::camera
