#pragma once

#ifdef __APPLE__
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#elif defined(__ANDROID__)
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "android/gl/gl3stub.h"

#endif

// OES扩展定义
#ifndef GL_TEXTURE_EXTERNAL_OES
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#endif
