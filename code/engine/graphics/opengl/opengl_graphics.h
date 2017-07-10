#pragma once
#include "graphics/graphics.h"
#include <GL/gl.h>

namespace engine
{
using namespace base;

class E_API OpenglGraphics : public Graphics
{
public:
	OpenglGraphics();
	~OpenglGraphics();
	bool initialize() override;
	void shutdown() override;
	Texture* createTexture() override;
	GpuProgram* createGpuProgram() override;
	GpuBuffer* createGpuBuffer() override;
	void present() override;
	void clear(
		ClearBufferFlags flags,
		const Color& clearColor = Color::black,
		f32 depth = 1.0f,
		u32 stencil = ~0) override;
	void setRenderTarget(
		Texture* renderTargetTexture,
		u32 cubeFaceIndex,
		u32 targetIndex) override;
	void beginDraw() override;
	void drawMesh(
		GpuProgram* gpuProgram,
		Mesh* mesh,
		GpuBuffer* instanceBuffer,
		u32 baseVertexIndex,
		u32 indexOffset,
		u32 vertexCount,
		u32 indexCount) override;
	void endDraw() override;
	void setViewport(const Rect& rc) override;
	void resetViewport() override;
	const Rect& getViewport() const override;
	void setScissor(const Rect& rc) override;
	Vec2 getDesktopScreenSize() override;
	void changeVideoMode(const VideoMode& newMode) override;
	void setFullScreen(bool fullScreen) override;
	void setWindow(Window* wnd) override;
	Window* getWindow() const override;
	GraphicsApiType getApiType() const override { return GraphicsApiType::OpenGL; };
	void commitBlendState() override;
	void commitRasterizerState() override;
	void commitDepthStencilState() override;

protected:
	void initializeGL();
	void releaseGL();
	void changeScreenGL();

	bool oglLowSpecs = false;
	GLuint oglFBO = 0;
};

}
