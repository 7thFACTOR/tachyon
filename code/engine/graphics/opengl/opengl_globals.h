#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WINDOWS
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/gl.h>

#ifdef _LINUX
#include <GL/glx.h>
#undef None
#undef Always
#undef Success
#undef Bool
#endif

#undef minCorner
#undef maxCorner
#define NOMINMAX

#include "graphics/types.h"

namespace engine
{
void openglCheckError(const String& where);
GLenum openglTranslateEnum(GpuBufferMapType type);
GLenum openglTranslateEnum(GpuBufferElementComponentType type);

extern u32 oglBlendType[];
extern u32 oglBlendOp[];
extern u32 oglCmpFuncType[];
extern u32 oglCullModeType[];
extern u32 oglFillModeType[];
extern u32 oglStencilOpType[];
extern u32 oglTextureAddressType[];
extern u32 oglTextureFilterType[];
extern u32 oglPrimitiveType[];
extern u32 oglGpuBufferType[];

#define E_USE_RENDER_STATISTICS
#ifdef _DEBUG
#define CHECK_OPENGL_ERROR { openglCheckError(String("File: ") << __FILE__ << ", line: " << __LINE__); };
#else
#define CHECK_OPENGL_ERROR
#endif
}
