#include <stdio.h>
#include <string.h>
#include "base/util.h"
#include "render/graphics_api.h"
#include "core/globals.h"
#include "base/assert.h"
#include "base/logger.h"
#include "resources/font_resource.h"
#include "resources/gpu_program_resource.h"
#include "resources/texture_resource.h"
#include "core/resource_repository.h"
#include "base/math/plane.h"
#include "base/math/conversion.h"
#include "base/math/util.h"
#include "base/math/intersection.h"
#include "render/texcoord.h"
#include "base/window.h"
#include "render/aux_renderer.h"
#include "render/resources/managers/gpu_program_manager.h"
#include "render/resources/managers/texture_manager.h"
#include "render/resources/managers/mesh_manager.h"

namespace engine
{
Graphics::Graphics()
{
	videoAdapterIndex = 0;
	osWindow = nullptr;
	memset(renderTarget, 0, sizeof(renderTarget[0]) * maxRenderTargetCount);
	memset(renderTargetCubeFaceIndex, 0, sizeof(renderTargetCubeFaceIndex[0]) * maxRenderTargetCount);
	dirtyMatrixFlags.flags = 0;
}

Graphics::~Graphics()
{}

bool Graphics::initialize()
{
	createAuxRenderer();
	getShapeRenderer()->loadAuxGpuProgram();
	getResources().registerResourceTypeRepository(new TextureRepository());
	getResources().registerResourceTypeRepository(new GpuProgramRepository());
	getResources().registerResourceTypeRepository(new MeshRepository());

	getResources().load(hashStringSbdm("meshes/teapot.mesh"));
	getResources().load(hashStringSbdm("gpu_programs/default.gpu_program"));

	return true;
}

bool Graphics::shutdown()
{
	deleteAuxRenderer();

	return true;
}

void Graphics::update()
{}

u32 Graphics::getVideoAdapterIndex() const
{
	return videoAdapterIndex;
}

const VideoMode& Graphics::getVideoMode() const
{
	return videoMode;
}

const Array<VideoAdapter>& Graphics::getVideoAdapters() const
{
	return videoAdapters;
}

void Graphics::changeVideoMode(const VideoMode& newMode)
{
	videoMode = newMode;
}

void Graphics::setFullScreen(bool fullScreen)
{
	videoMode.fullScreen = fullScreen;
}

void Graphics::setWindow(Window* wnd)
{
	osWindow = wnd;
}

u32 Graphics::registerSystemTexture(ResourceId tex, const String& name)
{
	static u32 nextTexId = 1;

	if (getSystemTexture(name))
	{
		B_LOG_WARNING("System texture named '%s' already registered", name);
		return 0;
	}

	u32 id = nextTexId++;

	SystemTexture stex;

	stex.id = id;
	stex.texture = tex;
	stex.name = name;
	systemTextures[id] = stex;

	return id;
}

void Graphics::unregisterSystemTexture(u32 id)
{
	auto iter = systemTextures.begin();

	while (iter != systemTextures.end())
	{
		if (iter->key == id)
		{
			systemTextures.erase(iter);
			return;
		}

		++iter;
	}
}

ResourceId Graphics::getSystemTexture(u32 id)
{
	auto iter = systemTextures.find(id);

	if(iter == systemTextures.end())
		return invalidResourceId;

	return iter->value.texture;
}

ResourceId Graphics::getSystemTexture(const String& name)
{
	auto iter = systemTextures.begin();

	while (iter != systemTextures.end())
	{
		if (iter->value.name == name)
		{
			return iter->value.texture;
		}

		++iter;
	}

	return invalidResourceId;
}

const Map<u32, SystemTexture>& Graphics::getSystemTextures() const
{
	return systemTextures;
}

BlendState& Graphics::getBlendState()
{
	return blendState;
}

RasterizerState& Graphics::getRasterizerState()
{
	return rasterizerState;
}

DepthStencilState& Graphics::getDepthStencilState()
{
	return depthStencilState;
}

ResourceId Graphics::getRenderTargetAt(u32 targetIndex) const
{
	return renderTarget[targetIndex];
}

void Graphics::setWorldMatrix(const Matrix& mtx)
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

void Graphics::setViewMatrix(const Matrix& mtx)
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

void Graphics::setProjectionMatrix(const Matrix& mtx)
{
	projectionMatrix = mtx;
	dirtyMatrixFlags.worldProjection = 1;
	dirtyMatrixFlags.viewProjection = 1;
	dirtyMatrixFlags.worldViewProjection = 1;
}

void Graphics::setTextureMatrix(const Matrix& rMtx)
{
	textureMatrix = rMtx;
}

const Matrix& Graphics::getWorldMatrix() const
{
	return worldMatrix;
}

const Matrix& Graphics::getViewMatrix() const
{
	return viewMatrix;
}

const Matrix& Graphics::getProjectionMatrix() const
{
	return projectionMatrix;
}

const Matrix& Graphics::getTextureMatrix() const
{
	return textureMatrix;
}

GraphicsApiType Graphics::getApiType() const
{
	return apiType;
}

void Graphics::bindGpuProgram(ResourceId resId)
{}

void Graphics::unbindGpuProgram()
{}

//---

// The Null render manager, used when engine runs as server, when you don't need rendering
class ENGINE_API NullRenderManager : public Graphics
{
public:
	B_DECLARE_CLASS(NullRenderManager);

	NullRenderManager()
	{
		apiType = Graphics::None;
	}

	~NullRenderManager()
	{}

	GpuBuffer* createGpuBuffer() override
	{
		return nullptr;
	}
	void setRenderTarget(ResourceId texture, u32 cubeFaceIndex = 0, u32 targetIndex = 0) override
	{
		return;
	}

	void present() override
	{
		return;
	}

	void clear(ClearBufferFlags flags, const Color& clearColor = Color::black, f32 depth = 1.0f, u32 stencil = ~0) override
	{}

	void beginDraw() override
	{}

	void drawGeometry(
		ResourceId program,
		Geometry* geomBuffer,
		GpuBuffer* instanceBuffer = nullptr,
		u32 baseVertexIndex = 0,
		u32 indexOffset = 0,
		u32 vertexCount = 0,
		u32 indexCount = 0) override
	{}

	void endDraw() override
	{}

	void setViewport(const Rect& rc) override
	{}

	void resetViewport() override
	{}

	void setScissor(const Rect& rc) override
	{}

	void commitBlendState() override
	{}

	void commitRasterizerState() override
	{}

	void commitDepthStencilState() override
	{}
};

B_REGISTER_CLASS(NullRenderManager);
NullRenderManager checkCompileNullRenderManager;

}