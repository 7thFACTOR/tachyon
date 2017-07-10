#include "opengl_globals.h"
#include "game/components/rendering/camera.h"
#include "render/texcoord.h"
#include "render/shape_renderer.h"
#include "render/resources/gpu_program_resource.h"
#include "game/resources/font_resource.h"
#include "base/window.h"
#include "base/math/conversion.h"
#include "base/logger.h"
#include "base/util.h"
#include "base/variable.h"
#include "core/globals.h"
#include "base/application.h"
#include "core/resource_repository.h"
#include "render/resources/mesh_resource.h"
#include "opengl_graphics.h"
#include "opengl_texture.h"
#include "opengl_gpu_program.h"
#include "opengl_gpu_buffer.h"
#include "base/timer.h"
#include "base/variable.h"
#include "3rdparty/SDL2/include/SDL.h"

namespace engine
{
using namespace base;

B_DEFINE_CLASS(OpenGLGraphics);

OpenGLGraphics* oglGraphics = nullptr;

#ifdef BASE_PLATFORM_LINUX
int singleBufferAttributess[] = 
{
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE,   GLX_RGBA_BIT,
	GLX_RED_SIZE,      1,   /* Request a single buffered color buffer */
	GLX_GREEN_SIZE,    1,   /* with the maximum number of color bits  */
	GLX_BLUE_SIZE,     1,   /* for each component                     */
	None
};

int doubleBufferAttributes[] = 
{
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE,   GLX_RGBA_BIT,
	GLX_DOUBLEBUFFER,  True,  /* Request a double-buffered color buffer with */
	GLX_RED_SIZE,      1,     /* the maximum number of bits per component    */
	GLX_GREEN_SIZE,    1, 
	GLX_BLUE_SIZE,     1,
	None
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
#ifdef BASE_PLATFORM_WINDOWS
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
#ifdef BASE_PLATFORM_WINDOWS
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

	const char *extensions = (char*)glGetString(GL_EXTENSIONS);

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

OpenGLGraphics::OpenGLGraphics()
	: Graphics()
{
	oglFBO = 0;
	window = nullptr;
	oglGraphics = this;
	sdlGlContext = 0;
}

OpenGLGraphics::~OpenGLGraphics()
{
	B_LOG_INFO("Destroying OpenGL graphics");
	oglGraphics = nullptr;
}

bool OpenGLGraphics::initialize()
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

void OpenGLGraphics::shutdown()
{
	releaseGL();
}

bool OpenGLGraphics::initializeGL()
{
	releaseGL();
	B_ASSERT(window);

	if (!window)
	{
		B_LOG_INFO("Could not create context, no render window was provided!");
		return false;
	}

	videoMode.fullScreen = getVariableRegistry().getVariableValue<bool>("gfx_fullscreen");
	auto sdlWnd = (SDL_Window*)window->getSdlHandle();
	sdlGlContext = SDL_GL_CreateContext(sdlWnd);

	SDL_GL_MakeCurrent(sdlWnd, sdlGlContext);

	if (glewInit() != GLEW_OK)
	{
		B_LOG_ERROR("GLEW initialize error");
	}

	oglLowSpecs = true;

	String ver = (const char*)glGetString(GL_VERSION);
	Array<String> verNumbers;

	if (explodeString(ver, verNumbers, "."))
	{
		if (atoi(verNumbers[0].charPtr()) > 2)
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

	window->focus();
	window->setFullscreen(videoMode.fullScreen);
	
	getGraphics().getDepthStencilState().depthTest = true;
	getGraphics().getDepthStencilState().depthWrite = true;
	getGraphics().getDepthStencilState().depthFunc = CompareFunction::LessEqual;
	getGraphics().getRasterizerState().cullMode = CullMode::CCW;
	getGraphics().resetViewport();
	setVSyncOpenGL(getVariableRegistry().getVariableValue<bool>("gfx_vsync"));

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	CHECK_OPENGL_ERROR;

	return true;
}

bool OpenGLGraphics::releaseGL()
{
	if (oglFBO)
	{
		glDeleteFramebuffersEXT(1, &oglFBO);
		oglFBO = 0;
	}

	return true;
}

void OpenGLGraphics::setFullScreen(bool fullscreen)
{
	videoMode.fullScreen = fullscreen;
	changeScreenGL();
}

void OpenGLGraphics::setWindow(Window* wnd)
{
	window = wnd;
}

Window* OpenGLGraphics::getWindow() const
{
	return window;
}

GraphicsApiType OpenGLGraphics::getApiType() const
{
	return oglLowSpecs ? GraphicsApiType::OpenGLES : GraphicsApiType::OpenGL;
}

BlendState& OpenGLGraphics::getBlendState()
{
	return blendState;
}

RasterizerState& OpenGLGraphics::getRasterizerState()
{
	return rasterizerState;
}

DepthStencilState& OpenGLGraphics::getDepthStencilState()
{
	return depthStencilState;
}

void OpenGLGraphics::setWorldMatrix(const Matrix& mtx)
{
	worldMatrix = mtx;
	dirtyMatrixFlags.worldInverse = 1;
	dirtyMatrixFlags.worldInverseTranspose = 1;
	dirtyMatrixFlags.worldView = 1;
	dirtyMatrixFlags.worldViewInverse = 1;
	dirtyMatrixFlags.worldViewInverseTranspose = 1;
	dirtyMatrixFlags.worldProjection = 1;
	dirtyMatrixFlags.worldViewProjection = 1;
}

void OpenGLGraphics::setViewMatrix(const Matrix& mtx)
{
	viewMatrix = mtx;
	dirtyMatrixFlags.viewInverse = 1;
	dirtyMatrixFlags.viewInverseTranspose = 1;
	dirtyMatrixFlags.viewRotation = 1;
	dirtyMatrixFlags.worldView = 1;
	dirtyMatrixFlags.worldViewInverse = 1;
	dirtyMatrixFlags.worldViewInverseTranspose = 1;
	dirtyMatrixFlags.viewProjection = 1;
	dirtyMatrixFlags.worldViewProjection = 1;
}

void OpenGLGraphics::setProjectionMatrix(const Matrix& mtx)
{
	projectionMatrix = mtx;
	dirtyMatrixFlags.worldProjection = 1;
	dirtyMatrixFlags.viewProjection = 1;
	dirtyMatrixFlags.worldViewProjection = 1;
}

void OpenGLGraphics::setTextureMatrix(const Matrix& mtx)
{
	textureMatrix = mtx;
}

const Matrix& OpenGLGraphics::getWorldMatrix() const
{
	return worldMatrix;
}

const Matrix& OpenGLGraphics::getViewMatrix() const
{
	return viewMatrix;
}

const Matrix& OpenGLGraphics::getProjectionMatrix() const
{
	return projectionMatrix;
}

const Matrix& OpenGLGraphics::getTextureMatrix() const
{
	return textureMatrix;
}

void OpenGLGraphics::changeVideoMode(const VideoMode& newMode)
{
	changeScreenGL();
}

void OpenGLGraphics::present()
{
	if (!window)
		return;

	SDL_GL_SwapWindow((SDL_Window*)window->getSdlHandle());
}

void OpenGLGraphics::commitBlendState()
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

void OpenGLGraphics::commitRasterizerState()
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

void OpenGLGraphics::commitDepthStencilState()
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
	CHECK_OPENGL_ERROR;
}

void OpenGLGraphics::setBuiltInGpuProgramConstant(GpuProgram* program, GpuProgramConstantType type)
{
	//TODO: maybe use some table for these? or a ptr to func to set them, saving some time searching in the switch-case
	// GpuConstantUploader<GpuConstantType>
	switch (type)
	{
	case GpuProgramConstantType::WorldViewProjectionMatrix:
		if (dirtyMatrixFlags.worldViewProjection)
		{
			worldViewProjectionMatrix = worldMatrix;
			worldViewProjectionMatrix = worldViewProjectionMatrix * viewMatrix;
			worldViewProjectionMatrix = worldViewProjectionMatrix * projectionMatrix;
			dirtyMatrixFlags.worldViewProjection = 0;
		}

		program->setMatrixUniform("mtxWorldViewProjection", worldViewProjectionMatrix);
		break;

	case GpuProgramConstantType::WorldMatrix:
		program->setMatrixUniform("mtxWorld", worldMatrix);
		break;

	case GpuProgramConstantType::ProjectionMatrix:
		program->setMatrixUniform("mtxProjection", projectionMatrix);
		break;

	case GpuProgramConstantType::WorldProjectionMatrix:
		if (dirtyMatrixFlags.worldProjection)
		{
			worldProjectionMatrix = worldMatrix;
			worldProjectionMatrix = worldProjectionMatrix * projectionMatrix;
			dirtyMatrixFlags.worldProjection = 0;
		}

		program->setMatrixUniform("mtxWorldProjection", worldProjectionMatrix);
		break;

	case GpuProgramConstantType::ViewProjectionMatrix:
		if (dirtyMatrixFlags.viewProjection)
		{
			viewProjectionMatrix = viewMatrix;
			viewProjectionMatrix = viewProjectionMatrix * projectionMatrix;
			dirtyMatrixFlags.viewProjection = 0;
		}

		program->setMatrixUniform("mtxViewProjection", viewProjectionMatrix);
		break;

	case GpuProgramConstantType::ViewMatrix:
		program->setMatrixUniform("mtxView", viewMatrix);
		break;

	case GpuProgramConstantType::WorldInverseMatrix:
		if (dirtyMatrixFlags.worldInverse)
		{
			worldInverseMatrix = worldMatrix;
			worldInverseMatrix.invert();
			dirtyMatrixFlags.worldInverse = 0;
		}

		program->setMatrixUniform("mtxWorldInverse", worldInverseMatrix);
		break;

	case GpuProgramConstantType::WorldInverseTransposeMatrix:
		if (dirtyMatrixFlags.worldInverseTranspose)
		{
			worldInverseTransposeMatrix = worldMatrix;
			worldInverseTransposeMatrix.invert();
			worldInverseTransposeMatrix.transpose();
			dirtyMatrixFlags.worldInverseTranspose = 0;
		}

		program->setMatrixUniform("mtxWorldInverseTranspose", worldInverseTransposeMatrix);
		break;

	case GpuProgramConstantType::ViewInverseMatrix:
		if (dirtyMatrixFlags.viewInverse)
		{
			viewInverseMatrix = viewMatrix;
			viewInverseMatrix.invert();
			dirtyMatrixFlags.viewInverse = 0;
		}

		program->setMatrixUniform("mtxViewInverse", viewInverseMatrix);
		break;

	case GpuProgramConstantType::ViewInverseTransposeMatrix:
		if (dirtyMatrixFlags.viewInverseTranspose)
		{
			viewInverseTransposeMatrix = viewMatrix;
			viewInverseTransposeMatrix.invert();
			viewInverseTransposeMatrix.transpose();
			dirtyMatrixFlags.viewInverseTranspose = 0;
		}

		program->setMatrixUniform("mtxViewInverseTranspose", viewInverseTransposeMatrix);
		break;

	case GpuProgramConstantType::ViewRotationMatrix:
		if (dirtyMatrixFlags.viewRotation)
		{
			viewRotationMatrix = viewMatrix.getRotationOnly();
			dirtyMatrixFlags.viewRotation = 0;
		}

		program->setMatrixUniform("mtxViewRotation", viewRotationMatrix);
		break;

	case GpuProgramConstantType::WorldViewMatrix:
		if (dirtyMatrixFlags.worldView)
		{
			worldViewMatrix = worldMatrix;
			worldViewMatrix = worldViewMatrix * viewMatrix;
			dirtyMatrixFlags.worldView = 0;
		}

		program->setMatrixUniform("mtxWorldView", worldViewMatrix);
		break;

	case GpuProgramConstantType::WorldViewInverseMatrix:
		if (dirtyMatrixFlags.worldViewInverse)
		{
			worldViewInverseMatrix = worldMatrix * viewMatrix;
			worldViewInverseMatrix.invert();
			dirtyMatrixFlags.worldViewInverse = 0;
		}

		program->setMatrixUniform("mtxWorldViewInverse", worldViewInverseMatrix);
		break;

	case GpuProgramConstantType::WorldViewInverseTransposeMatrix:
		if (dirtyMatrixFlags.worldViewInverseTranspose)
		{
			worldViewInverseTransposeMatrix = worldMatrix;
			worldViewInverseTransposeMatrix = worldViewInverseTransposeMatrix * viewMatrix;
			worldViewInverseTransposeMatrix.invert();
			worldViewInverseTransposeMatrix.transpose();
			dirtyMatrixFlags.worldViewInverseTranspose = 0;
		}

		program->setMatrixUniform("mtxWorldViewInverseTranspose", worldViewInverseTransposeMatrix);
		break;

	case GpuProgramConstantType::TextureMatrix:
		program->setMatrixUniform("mtxTexture", textureMatrix);
		break;

	case GpuProgramConstantType::EngineTime:
		program->setFloatUniform("time", (f32)getClock().getMainTimer().getTime());
		break;

	case GpuProgramConstantType::EngineFrameDeltaTime:
		program->setFloatUniform("frameDeltaTime", (f32)getClock().getTimer(SystemTimer::game)->getDelta());
		break;

	case GpuProgramConstantType::UnitDomainRotatorTime:
		//TODO
		break;

	case GpuProgramConstantType::EngineFps:
		//TODO
		break;

	case GpuProgramConstantType::RenderTargetSize:
	{
		f32 rtSize[2] = { renderTarget[0]->getWidth(), renderTarget[0]->getHeight() };
		program->setFloatArrayUniform("renderTargetSize", rtSize, 2);
		break;
	}

	case GpuProgramConstantType::ViewportSize:
	{
		Rect vp = getViewport();
		f32 vpSize[2] = { vp.width, getViewport().height };
		program->setFloatArrayUniform("renderTargetSize", vpSize, 2);
		break;
	}

	case GpuProgramConstantType::CameraFov:
		//TODO
		break;

	case GpuProgramConstantType::CameraNearPlane:
		//TODO
		break;

	case GpuProgramConstantType::CameraFarPlane:
		//TODO
		break;

	case GpuProgramConstantType::CameraUpAxis:
		//TODO
		break;

	case GpuProgramConstantType::CameraPosition:
	{
		Vec3 camPos = viewMatrix.getTranslation();
		program->setFloatArrayUniform("cameraPosition", (f32*)&camPos, 3);
		break;
	}

	case GpuProgramConstantType::ColorTexture:
		//TODO
		break;

	case GpuProgramConstantType::DepthTexture:
		//TODO
		break;

	case GpuProgramConstantType::SystemTexture:
		//TODO
		break;

	default:
		B_ASSERT(!"No known built-in gpu program constant");
	}
}

void OpenGLGraphics::clear(ClearBufferFlags flags, const Color& color, f32 depth, u32 stencil)
{
	u32 clearFlags = 0;

	if (flags.color)
	{
		clearFlags |= GL_COLOR_BUFFER_BIT;
	}

	if (flags.depth)
	{
		clearFlags |= GL_DEPTH_BUFFER_BIT;
	}

	if (flags.stencil)
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

void OpenGLGraphics::setRenderTarget(Texture* target, u32 cubeFaceIndex, u32 targetIndex)
{
	OpenGLTexture* oglTex = (OpenGLTexture*)target;

	if (!oglTex)
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
	renderTarget[targetIndex] = oglTex;
}

void OpenGLGraphics::beginDraw()
{}

void OpenGLGraphics::endDraw()
{}

void OpenGLGraphics::setViewport(const Rect& rc)
{
	viewport = rc;
	glViewport(rc.x, rc.y, rc.width, rc.height);
	CHECK_OPENGL_ERROR;
}

void OpenGLGraphics::resetViewport()
{
	B_ASSERT(window);
	auto wsize = window->getClientSize();
	setViewport(Rect(0, 0, wsize.x, wsize.y));
}

const Rect& OpenGLGraphics::getViewport() const
{
	return viewport;
}

void OpenGLGraphics::setScissor(const Rect& rc)
{
	scissor = rc;
	glScissor(rc.x, rc.y, rc.width, rc.height);
	CHECK_OPENGL_ERROR;
}

Vec2 OpenGLGraphics::getDesktopScreenSize()
{
	return Vec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}

u32 OpenGLGraphics::getVideoAdapterIndex() const
{
	return videoAdapterIndex;
}

const Array<VideoAdapter>& OpenGLGraphics::getVideoAdapters() const
{
	return videoAdapters;
}

const VideoMode& OpenGLGraphics::getVideoMode() const
{
	return videoMode;
}

void OpenGLGraphics::changeScreenGL()
{
#ifdef BASE_PLATFORM_WINDOWS
	if (videoMode.fullScreen)
	{
		if (window)
		{
			window->setPosition(Vec2());
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

void OpenGLGraphics::drawMesh(
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

	OpenGLGpuBuffer* oglVertexBuffer = (OpenGLGpuBuffer*)mesh->getVertexBuffer();
	OpenGLGpuBuffer* oglIndexBuffer = (OpenGLGpuBuffer*)mesh->getIndexBuffer();
	OpenGLGpuBuffer* oglInstanceBuffer = (OpenGLGpuBuffer*)instanceBuffer;
	OpenGLGpuProgram* oglProgram = (OpenGLGpuProgram*)program;

	if (!oglProgram)
	{
		return;
	}

	oglVertexBuffer->setupVertexDeclarationGL(*oglProgram, baseVertexIndex);

	if (oglInstanceBuffer)
	{
		oglInstanceBuffer->setupVertexDeclarationGL(*oglProgram, oglVertexBuffer->oglLastUsedAttribIndex);
	}

	vertexCount = vertexCount ? vertexCount : oglVertexBuffer->getElementCount();
	indexCount = indexCount ? indexCount : oglIndexBuffer->getElementCount();
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
				oglInstanceBuffer->getElementCount());
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

GpuBuffer* OpenGLGraphics::createGpuBuffer()
{
	return new OpenGLGpuBuffer;
}

GpuProgram* OpenGLGraphics::createGpuProgram()
{
	return new OpenGLGpuProgram;
}

Texture* OpenGLGraphics::createTexture()
{
	return new OpenGLTexture;
}

void OpenGLGraphics::update()
{
	Array<ResourceId> ids;

	getResources().fetchLoadedResourceIds(ResourceType::GpuProgram, ids);

	for (auto resId : ids)
	{
		OpenGLGpuProgram* program = new OpenGLGpuProgram();

		gpuPrograms[resId] = program;
		program->setResource(resId);
	}

	getResources().fetchLoadedResourceIds(ResourceType::Texture, ids);

	for (auto resId : ids)
	{
		OpenGLTexture* tex = new OpenGLTexture();
		textures[resId] = tex;
		tex->setResource(resId);
	}

	getResources().fetchLoadedResourceIds(ResourceType::Mesh, ids);

	for (auto resId : ids)
	{
		Mesh* mesh = new Mesh();
		meshes[resId] = mesh;

		mesh->setResource(resId);
	}
}

}