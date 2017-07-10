#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef BASE_PLATFORM_WINDOWS
#include <windows.h>
#endif

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#ifdef BASE_PLATFORM_LINUX
#include <GL/glx.h>
#endif

#undef minCorner
#undef maxCorner

#define NOMINMAX

#include "render/types.h"

namespace engine
{
void openglCheckError(const String& where);
GLenum openglTranslateEnum(GpuBufferMapType type);
GLenum openglTranslateEnum(GpuBufferElementComponentType type);

#define E_USE_RENDER_STATISTICS
#ifndef NDEBUG
#define CHECK_OPENGL_ERROR { openglCheckError(String("File: ") << __FILE__ << ", line: " << __LINE__); };
#else
#define CHECK_OPENGL_ERROR
#endif
}