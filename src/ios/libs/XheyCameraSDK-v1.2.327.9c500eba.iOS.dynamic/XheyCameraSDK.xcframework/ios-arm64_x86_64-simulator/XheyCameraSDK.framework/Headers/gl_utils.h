#pragma once

#include "common/matrix.h"
#include "common/media_types.h"
#include "gl.h"

namespace xhey::camera {

#define GLES_SHADER_CODE_WITH_PRECISION_HIGHP(x) "precision highp float;\n" #x
#define GLES_SHADER_CODE_WITH_PRECISION_MEDIUMP(x) "precision mediump float;\n" #x

extern const char *kDefaultVertexShader;
extern const char *kDefaultFragmentShader;
extern const char *kPositionAttributeName;
extern const char *kTextureCoordinateAttributeName;
extern const char *kMVPMatrixUniformName;
extern const char *kInputImageTextureUniformName;
extern const GLfloat kDefaultVertexData[];
extern const GLfloat kDefaultTextureCoordinateData[];

static constexpr int kTextureCoordinateDataSize = 8;

GLenum CheckGlError(const char *message);
bool CheckGlShaderCompilation(const char *message, GLuint shader);
bool CheckGlProgramLink(const char *message, GLuint program);

float GetEsVersion();
bool IsEs3Supported();

GLuint LoadShader(GLenum type, const char *source);

class ShaderProgram;
void SetupGlProgram(ShaderProgram *program, VideoFrame *video_frame, GLint position_attribute_location,
                    GLint texture_coordinate_attribute_location, GLint mvp_matrix_uniform_location, ViewPort view_port,
                    bool render_to_texture, GLfloat *texture_coordinate_data);

const GLfloat *GetYUV2RGBMatrix(ColorSpace color_space, ColorRange color_range);
const GLfloat *GetRGB2YUVMatrix(ColorSpace color_space, ColorRange color_range);

} // namespace xhey::camera
