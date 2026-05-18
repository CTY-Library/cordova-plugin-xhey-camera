#pragma once

#include "gl.h"

namespace xhey::camera {

class ShaderProgram {
public:
    ShaderProgram(const char *vertex_shader, const char *fragment_shader);
    ~ShaderProgram();

    GLuint GetProgramId() const { return program_id_; }

    void Use();

private:
    GLuint program_id_ = 0;
};

} // namespace xhey::camera
