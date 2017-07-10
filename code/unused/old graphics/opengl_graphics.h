#pragma once
#include "opengl_globals.h"
#ifdef BASE_PLATFORM_WINDOWS
#include <windows.h>
#endif
#include "render/graphics.h"
#include "base/statistics.h"
#include "base/class_registry.h"
#include "3rdparty/SDL2/include/SDL.h"

namespace engine
{
class OpenGLTexture;
class OpenGLGpuBuffer;
class OpenGLGpuProgram;

class ENGINE_API OpenGLGraphics : public Graphics
{
public:
	B_DECLARE_DERIVED_CLASS(OpenGLGraphics, Graphics);

	friend class OpenGLGpuProgram;
	friend class OpenGLTexture;

	OpenGLGraphics();
	~OpenGLGraphics();

	bool initialize() override;
	void shutdown() override;
	GpuBuffer* createGpuBuffer() override;
	Texture* createTexture() override;
	GpuProgram* createGpuProgram() override;
	void update() override;
	void present() override;
	void clear(
		ClearBufferFlags flags,
		const Color& clearColor,
		f32 depth,
		u32 stencil) override;
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
	u32 getVideoAdapterIndex() const override;
	const Array<VideoAdapter>& getVideoAdapters() const override;
	const VideoMode& getVideoMode() const override;
	void changeVideoMode(const VideoMode& newMode) override;
	void setFullScreen(bool fullScreen) override;
	void setWindow(Window* wnd) override;
	Window* getWindow() const override;
	GraphicsApiType getApiType() const override;
	BlendState& getBlendState() override;
	RasterizerState& getRasterizerState() override;
	DepthStencilState& getDepthStencilState() override;
	void setWorldMatrix(const Matrix& mtx) override;
	void setViewMatrix(const Matrix& mtx) override;
	void setProjectionMatrix(const Matrix& mtx) override;
	void setTextureMatrix(const Matrix& mtx) override;
	const Matrix& getWorldMatrix() const override;
	const Matrix& getViewMatrix() const override;
	const Matrix& getProjectionMatrix() const override;
	const Matrix& getTextureMatrix() const override;
	void commitBlendState() override;
	void commitRasterizerState() override;
	void commitDepthStencilState() override;
	void setBuiltInGpuProgramConstant(GpuProgram* program, GpuProgramConstantType type) override;
	Dictionary<ResourceId, Mesh*>& getMeshes() override { return meshes; }
	Dictionary<ResourceId, GpuProgram*>& getGpuPrograms() override { return gpuPrograms; }
	Dictionary<ResourceId, Texture*>& getTextures() override { return textures; }

	bool initializeGL();
	bool releaseGL();
	void changeScreenGL();

protected:
	bool oglLowSpecs = false;
	GLuint oglFBO;
	SDL_GLContext sdlGlContext;
	Dictionary<ResourceId, GpuProgram*> gpuPrograms;
	Dictionary<ResourceId, Texture*> textures;
	Dictionary<ResourceId, Mesh*> meshes;
	Array<VideoAdapter> videoAdapters;
	u32 videoAdapterIndex = 0;
	VideoMode videoMode;
	Window* window = nullptr;
	OpenGLTexture* renderTarget[maxRenderTargetCount];
	AntialiasLevel antialiasLevel;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	Matrix worldMatrix;
	Matrix textureMatrix;
	Matrix worldInverseMatrix;
	Matrix worldInverseTransposeMatrix;
	Matrix viewInverseMatrix;
	Matrix viewInverseTransposeMatrix;
	Matrix viewRotationMatrix;
	Matrix worldViewMatrix;
	Matrix worldViewInverseMatrix;
	Matrix worldViewInverseTransposeMatrix;
	Matrix worldProjectionMatrix;
	Matrix viewProjectionMatrix;
	Matrix worldViewProjectionMatrix;
	DirtyMatrixFlags dirtyMatrixFlags;
	BlendState blendState;
	RasterizerState rasterizerState;
	DepthStencilState depthStencilState;
	Rect viewport;
	Rect scissor;
	u32 renderTargetCubeFaceIndex[maxRenderTargetCount];
	StatCounter scTrisPerSec, scTrisPerFrame;
};

extern OpenGLGraphics* oglGraphics;
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

}