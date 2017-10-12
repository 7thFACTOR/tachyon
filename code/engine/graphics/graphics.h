// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "base/math/rect.h"
#include "base/math/color.h"
#include "base/dictionary.h"
#include "graphics/texcoord.h"
#include "graphics/video_adapter.h"
#include "graphics/types.h"
#include "core/types.h"
#include "core/resources/gpu_program_resource.h"

namespace engine
{
using namespace base;

class Mesh;
class Texture;
class GpuBuffer;
class GpuProgram;
class Window;

class E_API Graphics
{
public:
	Graphics() {}
	virtual ~Graphics() {}
	virtual bool initialize() = 0;
	virtual void shutdown();
	virtual Texture* createTexture() = 0;
	virtual GpuProgram* createGpuProgram() = 0;
	virtual GpuBuffer* createGpuBuffer() = 0;
	virtual void update();
	virtual void present() = 0;
	virtual void clear(
		ClearBufferFlags flags,
		const Color& clearColor = Color::black,
		f32 depth = 1.0f,
		u32 stencil = ~0) = 0;
	virtual void setRenderTarget(
		Texture* renderTargetTexture,
		u32 cubeFaceIndex,
		u32 targetIndex) = 0;
	virtual void beginDraw() = 0;
	virtual void drawMesh(
		GpuProgram* gpuProgram,
		Mesh* mesh,
		GpuBuffer* instanceBuffer,
		u32 baseVertexIndex,
		u32 indexOffset,
		u32 vertexCount,
		u32 indexCount) = 0;
	virtual void endDraw() = 0;

	virtual void setViewport(const Rect& rc) = 0;
	virtual void resetViewport() = 0;
	virtual const Rect& getViewport() const = 0;
	virtual void setScissor(const Rect& rc) = 0;
	virtual Vec2 getDesktopScreenSize() = 0;
	virtual u32 getVideoAdapterIndex() const;
	virtual const Array<VideoAdapter>& getVideoAdapters() const;
	virtual const VideoMode& getVideoMode() const;
	virtual void changeVideoMode(const VideoMode& newMode) = 0;
	virtual void setFullScreen(bool fullScreen) = 0;
	virtual void setWindow(Window* wnd) = 0;
	virtual Window* getWindow() const = 0;
	virtual GraphicsApiType getApiType() const = 0;
	virtual void setWorldMatrix(const Matrix& mtx);
	virtual void setViewMatrix(const Matrix& mtx);
	virtual void setProjectionMatrix(const Matrix& mtx);
	virtual void setTextureMatrix(const Matrix& mtx);
	virtual const Matrix& getWorldMatrix() const;
	virtual const Matrix& getViewMatrix() const;
	virtual const Matrix& getProjectionMatrix() const;
	virtual const Matrix& getTextureMatrix() const;
	virtual void commitBlendState() = 0;
	virtual void commitRasterizerState() = 0;
	virtual void commitDepthStencilState() = 0;
	virtual void setBuiltInGpuProgramConstant(GpuProgram* program, GpuProgramConstantType type);

public:
	Dictionary<ResourceId, GpuProgram*> gpuPrograms;
	Dictionary<ResourceId, Texture*> textures;
	Dictionary<ResourceId, Mesh*> meshes;
	BlendState blendState;
	RasterizerState rasterizerState;
	DepthStencilState depthStencilState;

protected:
	Array<VideoAdapter> videoAdapters;
	u32 videoAdapterIndex = 0;
	VideoMode videoMode;
	Window* window = nullptr;
	Texture* renderTarget[maxRenderTargetCount] = { nullptr };
	AntialiasLevel antialiasLevel = AntialiasLevel::None;
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
	DirtyMatrixFlags dirtyMatrixFlags = DirtyMatrixFlags::All;
	Rect viewport;
	Rect scissor;
	u32 renderTargetCubeFaceIndex[maxRenderTargetCount];
};

}