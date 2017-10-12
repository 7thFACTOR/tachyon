// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "opengl_globals.h"
#include "graphics/opengl/opengl_graphics.h"
#include "graphics/opengl/opengl_texture.h"
#include "graphics/opengl/opengl_gpu_program.h"
#include "graphics/opengl/opengl_gpu_buffer.h"
#include "graphics/mesh.h"
#include "core/globals.h"
#include "base/variable.h"
#include "input/window.h"

namespace engine
{
using namespace base;

#ifdef _LINUX
int singleBufferAttributess[] = 
{
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE,   GLX_RGBA_BIT,
	GLX_RED_SIZE,      1,   /* Request a single buffered color buffer */
	GLX_GREEN_SIZE,    1,   /* with the maximum number of color bits  */
	GLX_BLUE_SIZE,     1,   /* for each component                     */
	0
};

int doubleBufferAttributes[] = 
{
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE,   GLX_RGBA_BIT,
	GLX_DOUBLEBUFFER,  True,  /* Request a double-buffered color buffer with */
	GLX_RED_SIZE,      1,     /* the maximum number of bits per component    */
	GLX_GREEN_SIZE,    1, 
	GLX_BLUE_SIZE,     1,
	0
};
#endif

u32 oglBlendType[] =
{
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA_SATURATE
};

u32 oglBlendOp[] =
{
	GL_NONE,
	GL_FUNC_ADD,
	GL_FUNC_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT,
	GL_MIN,
	GL_MAX
};

u32 oglCmpFuncType[] =
{
	GL_NEVER,
	GL_LESS,
	GL_EQUAL,
	GL_LEQUAL,
	GL_GREATER,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_ALWAYS
};

u32 oglCullModeType[] =
{
	GL_NONE,
	GL_BACK, // switched to back so force OGL to use LeftHand coord system
	GL_FRONT
};

u32 oglFillModeType[] =
{
	GL_POINT,
	GL_LINE,
	GL_FILL
};

u32 oglStencilOpType[] =
{
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	GL_INCR,//_WRAP,
	GL_DECR,//_WRAP,
	GL_INVERT,
	GL_INCR,
	GL_DECR
};

u32 oglTextureAddressType[] =
{
	GL_REPEAT,
	GL_CLAMP,
};

u32 oglTextureFilterType[] =
{
	GL_NEAREST_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR,
	GL_NEAREST,
	GL_LINEAR,
};

u32 oglPrimitiveType[] =
{
	GL_POINTS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP
};

u32 oglGpuBufferType[] =
{
	GL_ELEMENT_ARRAY_BUFFER,
	GL_ARRAY_BUFFER
};

//---

static void fillAdapterModes(VideoAdapter& videoAdapter)
{
#ifdef _WINDOWS
	i32 numMode = 0;
	DEVMODE mode;
	VideoMode videoMode;

	memset(&mode, 0, sizeof(DEVMODE));
	mode.dmSize = sizeof(DEVMODE);

	B_LOG_INFO("Supported video modes:");
	while (EnumDisplaySettings(nullptr, numMode, &mode))
	{
		if (mode.dmBitsPerPel >= 16)
		{
			videoMode.width  = mode.dmPelsWidth;
			videoMode.height = mode.dmPelsHeight;
			videoMode.bitsPerPixel = mode.dmBitsPerPel;
			videoMode.frequency = mode.dmDisplayFrequency;
			videoAdapter.videoModes.append(videoMode);
		}

		++numMode;
	}
#endif
}

void setVSyncOpenGL(bool sync)
{
#ifdef _WINDOWS
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

	const char* extensions = (char*)glGetString(GL_EXTENSIONS);

	if (strstr(extensions, "WGL_EXT_swap_control") == 0)
	{
		return;
	}
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(sync);
	}
#endif
}

//---

OpenglGraphics::OpenglGraphics()
{
	oglFBO = 0;
	window = nullptr;
}

OpenglGraphics::~OpenglGraphics()
{
	B_LOG_INFO("Destroying OpenGL graphics");
}

bool OpenglGraphics::initialize()
{
	VideoAdapter adapter;

	//TODO: choose ogl or ogles, based on gl version
	//TODO: enum devices
	adapter.name = "Primary Display Adapter";
	videoAdapters.clear();
	fillAdapterModes(adapter);
	videoAdapters.append(adapter);
	initializeGL();
	B_LOG_INFO("OpenGL graphics initialized.");

	return true;
}

void OpenglGraphics::shutdown()
{
	releaseGL();
}

Texture* OpenglGraphics::createTexture()
{
	return new OpenglTexture();
}

GpuProgram* OpenglGraphics::createGpuProgram()
{
	return new OpenglGpuProgram();
}

GpuBuffer* OpenglGraphics::createGpuBuffer()
{
	return new OpenglGpuBuffer();
}

void OpenglGraphics::initializeGL()
{
	releaseGL();
	B_ASSERT(window);

	if (!window)
	{
		B_LOG_INFO("Could not create context, no render window was provided!");
		return;
	}

	videoMode.fullScreen = getVariableRegistry().getVariableValue<bool>("gfx_fullscreen");

	if (glewInit() != GLEW_OK)
	{
		B_LOG_ERROR("GLEW initialize error");
	}

	oglLowSpecs = true;

	String ver = (const char*)glGetString(GL_VERSION);
	Array<String> verNumbers;

	if (explodeString(ver, verNumbers, "."))
	{
		if (atoi(verNumbers[0].c_str()) > 2)
		{
			oglLowSpecs = false;
		}
	}

	B_LOG_INFO("OpenGL Version: '" << ver << "'");

	if (oglLowSpecs)
	{
		B_LOG_INFO("Using low-spec OpenGL ES functions and extensions");
	}
	else
	{
		B_LOG_INFO("Using high-spec OpenGL functions and extensions");
	}

	if (glGenFramebuffers)
	{
		glGenFramebuffers(1, &oglFBO);
		CHECK_OPENGL_ERROR;
	}

	window->requestFocus();
	getGraphics().depthStencilState.depthTest = true;
	getGraphics().depthStencilState.depthWrite = true;
	getGraphics().depthStencilState.depthFunc = CompareFunction::LessEqual;
	getGraphics().rasterizerState.cullMode = CullMode::CCW;
	commitDepthStencilState();
	getGraphics().resetViewport();
	setVSyncOpenGL(getVariableRegistry().getVariableValue<bool>("gfx_vsync"));

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	CHECK_OPENGL_ERROR;
	B_LOG_INFO("OpenGL initialized");
}

void OpenglGraphics::releaseGL()
{
	if (oglFBO)
	{
		glDeleteFramebuffersEXT(1, &oglFBO);
		oglFBO = 0;
	}
}

void OpenglGraphics::setFullScreen(bool fullscreen)
{
	videoMode.fullScreen = fullscreen;
	changeScreenGL();
}

void OpenglGraphics::setWindow(Window* wnd)
{
	window = wnd;
}

Window* OpenglGraphics::getWindow() const
{
	return window;
}

void OpenglGraphics::changeVideoMode(const VideoMode& newMode)
{
	changeScreenGL();
}

void OpenglGraphics::present()
{
	if (!window)
		return;

	window->present();
}

void OpenglGraphics::commitBlendState()
{
	//TODO: glAlphaFunc(GL_LESS, pBlendState->alphaRef);
	
	blendState.renderTarget[0].enable 
		? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	CHECK_OPENGL_ERROR;
	glBlendEquationSeparate(
		oglBlendOp[(u32)blendState.renderTarget[0].blendOp],
		oglBlendOp[(u32)blendState.renderTarget[0].blendOpAlpha]);
	CHECK_OPENGL_ERROR;
	glBlendFuncSeparate(
		oglBlendType[(u32)blendState.renderTarget[0].srcBlend],
		oglBlendType[(u32)blendState.renderTarget[0].destBlend],
		oglBlendType[(u32)blendState.renderTarget[0].srcBlendAlpha],
		oglBlendType[(u32)blendState.renderTarget[0].destBlendAlpha]);
	CHECK_OPENGL_ERROR;
}

void OpenglGraphics::commitRasterizerState()
{
	rasterizerState.scissorTest ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
	CHECK_OPENGL_ERROR;
	GLint cull = oglCullModeType[(u32)rasterizerState.cullMode];

	if (cull == GL_NONE)
	{
		glDisable(GL_CULL_FACE);
		CHECK_OPENGL_ERROR;
	}
	else
	{
		glEnable(GL_CULL_FACE);
		CHECK_OPENGL_ERROR;
		glCullFace(cull);
		CHECK_OPENGL_ERROR;
	}

	glPolygonMode(GL_FRONT_AND_BACK, oglFillModeType[(int)rasterizerState.fillMode]);
	CHECK_OPENGL_ERROR;
}

void OpenglGraphics::commitDepthStencilState()
{
	depthStencilState.depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	CHECK_OPENGL_ERROR;
	depthStencilState.depthWrite ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
	CHECK_OPENGL_ERROR;
	depthStencilState.stencil ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
	CHECK_OPENGL_ERROR;
	glStencilFuncSeparate(
		GL_FRONT,
		oglCmpFuncType[(u32)depthStencilState.frontFace.function],
		depthStencilState.stencilRef,
		depthStencilState.stencilWriteMask);
	CHECK_OPENGL_ERROR;
	glStencilFuncSeparate(
		GL_BACK,
		oglCmpFuncType[(u32)depthStencilState.backFace.function],
		depthStencilState.stencilRef,
		depthStencilState.stencilWriteMask);
	CHECK_OPENGL_ERROR;
	glDepthFunc(oglCmpFuncType[(u32)depthStencilState.depthFunc]);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(1);
	CHECK_OPENGL_ERROR;
}

void OpenglGraphics::clear(ClearBufferFlags flags, const Color& color, f32 depth, u32 stencil)
{
	u32 clearFlags = 0;

	if (!!(flags & ClearBufferFlags::Color))
	{
		clearFlags |= GL_COLOR_BUFFER_BIT;
	}

	if (!!(flags & ClearBufferFlags::Depth))
	{
		clearFlags |= GL_DEPTH_BUFFER_BIT;
	}

	if (!!(flags & ClearBufferFlags::Stencil))
	{
		clearFlags |= GL_STENCIL_BUFFER_BIT;
	}

	glDepthMask(GL_TRUE);
	glClearColor(color.r, color.g, color.b, color.a);
	CHECK_OPENGL_ERROR;
	glClearDepth(depth);
	CHECK_OPENGL_ERROR;
	glClearStencil(stencil);
	CHECK_OPENGL_ERROR;

	if (clearFlags)
	{
		glClear(clearFlags);
		CHECK_OPENGL_ERROR;
	}
}

void OpenglGraphics::setRenderTarget(Texture* target, u32 cubeFaceIndex, u32 targetIndex)
{
	OpenglTexture* oglTex = (OpenglTexture*)target;

	if (!target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, oglFBO);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + targetIndex,
			GL_TEXTURE_2D,
			oglTex->oglTexId, 0);
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER,
			oglTex->oglDepthRBO);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + targetIndex);
	}

	renderTargetCubeFaceIndex[targetIndex] = cubeFaceIndex;
	renderTarget[targetIndex] = target;
}

void OpenglGraphics::beginDraw()
{}

void OpenglGraphics::endDraw()
{}

void OpenglGraphics::setViewport(const Rect& rc)
{
	viewport = rc;
	glViewport(rc.x, rc.y, rc.width, rc.height);
	CHECK_OPENGL_ERROR;
}

void OpenglGraphics::resetViewport()
{
	B_ASSERT(window);
	auto wsize = window->getSize();
	setViewport(Rect(0, 0, wsize.x, wsize.y));
}

const Rect& OpenglGraphics::getViewport() const
{
	return viewport;
}

void OpenglGraphics::setScissor(const Rect& rc)
{
	scissor = rc;
	glScissor(rc.x, rc.y, rc.width, rc.height);
	CHECK_OPENGL_ERROR;
}

Vec2 OpenglGraphics::getDesktopScreenSize()
{
#ifdef _WINDOWS
	return Vec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#elif defined(_LINUX)
	Display* d = XOpenDisplay(NULL);
	Screen*  s = DefaultScreenOfDisplay(d);
	return Vec2(s->width, s->height);
#else
	return Vec2();
#endif
}

void OpenglGraphics::changeScreenGL()
{
#ifdef _WINDOWS
	if (videoMode.fullScreen)
	{
		if (window)
		{
			window->setPosition(Vec2i());
		}

		DEVMODE dmScreenSettings;

		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = videoMode.width;
		dmScreenSettings.dmPelsHeight = videoMode.height;
		dmScreenSettings.dmBitsPerPel = videoMode.bitsPerPixel;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(
			&dmScreenSettings,
			CDS_FULLSCREEN)
				!=DISP_CHANGE_SUCCESSFUL)
		{
			B_LOG_ERROR("Could not initialize to full screen.");
		}
		else
		{
			B_LOG_INFO("Changed to full screen: " << videoMode.width << " x " << videoMode.height << " @ " << videoMode.bitsPerPixel);
		}
	}
	else
	{
		ChangeDisplaySettings(nullptr, 0);
	}
#else
	B_ASSERT_NOT_IMPLEMENTED;
#endif
	resetViewport();
}

void OpenglGraphics::drawMesh(
	GpuProgram* program,
	Mesh* mesh,
	GpuBuffer* instanceBuffer,
	u32 baseVertexIndex,
	u32 indexOffset,
	u32 vertexCount,
	u32 indexCount)
{
	B_ASSERT(program);
	B_ASSERT(mesh);

	OpenglGpuBuffer* oglVertexBuffer = (OpenglGpuBuffer*)mesh->getVertexBuffer();
	OpenglGpuBuffer* oglIndexBuffer = (OpenglGpuBuffer*)mesh->getIndexBuffer();
	OpenglGpuBuffer* oglInstanceBuffer = (OpenglGpuBuffer*)instanceBuffer;

	if (!program)
	{
		return;
	}

	oglVertexBuffer->setupVertexDeclarationGL((OpenglGpuProgram&)*program, baseVertexIndex);

	if (oglInstanceBuffer)
	{
		oglInstanceBuffer->setupVertexDeclarationGL((OpenglGpuProgram&)*program, oglVertexBuffer->oglLastUsedAttribIndex);
	}

	vertexCount = vertexCount ? vertexCount : oglVertexBuffer->elementCount;
	indexCount = indexCount ? indexCount : oglIndexBuffer->elementCount;
	u32 primitiveCount = 0;
	GLuint primType = GL_TRIANGLES;

	switch (oglIndexBuffer->primitiveType)
	{
	case RenderPrimitive::Points:
		primitiveCount = indexCount;
		primType = GL_POINTS;
		break;
	case RenderPrimitive::Lines:
		primitiveCount = indexCount / 2;
		primType = GL_LINES;
		break;
	case RenderPrimitive::LineStrip:
		primitiveCount = indexCount - 1;
		primType = GL_LINE_STRIP;
		break;
	case RenderPrimitive::Triangles:
		primitiveCount = indexCount / 3;
		break;
	case RenderPrimitive::TriangleStrip :
		primitiveCount = indexCount - 2;
		primType = GL_TRIANGLE_STRIP;
		break;
	}

	if (!primitiveCount)
	{
		return;
	}

	oglVertexBuffer->bind();

	if (indexCount)
	{
		oglIndexBuffer->bind();
		CHECK_OPENGL_ERROR;

		if (oglInstanceBuffer && glDrawElementsInstancedEXT)
		{
			glDrawElementsInstancedEXT(
				primType,
				indexCount,
				GL_UNSIGNED_INT,
				(char*)(indexOffset * sizeof(u32)),
				oglInstanceBuffer->elementCount);
			CHECK_OPENGL_ERROR;
		}
		else
		{
			glDrawElements(primType, indexCount, GL_UNSIGNED_INT, (char*)(indexOffset * sizeof(u32)));
			CHECK_OPENGL_ERROR;
		}
	}

	GLint numAttribs = 0;
	
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttribs);
	CHECK_OPENGL_ERROR;

	for (int i = 0; i < numAttribs; ++i)
	{
		glDisableVertexAttribArray(i);
		CHECK_OPENGL_ERROR;
	}
}

}
