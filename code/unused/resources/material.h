#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/resource.h"
#include "base/serialize.h"
#include "render/types.h"
#include "render/render_state.h"

namespace engine
{
class Texture;
class GpuProgram;
class GpuProgramRenderMethod;

enum EMaterialChunkIds
{
	eMaterialChunk_Header,
	eMaterialChunk_Layers
};

#define E_WIP_MATERIAL_FILENAME "materials/wip.nmaterial"
#define E_MISSING_MATERIAL_FILENAME "materials/missing.nmaterial"

struct TextureSamplerInfo
{
	TextureSamplerInfo()
		: texture(kInvalidResourceId)
	{
	}

	ResourceId texture;
	String fileName;
	String samplerName;
	ETextureAddressType addressU, addressV, addressW;
	ETextureFilterType filter;
	f32 minLod, maxLod, mipMapLodBias;
};

class E_API MaterialLayer : public ISerializable, public Naming
{
public:
	MaterialLayer();
	virtual ~MaterialLayer();

	bool load(Stream* pStream);
	void loadResources();
	void unloadResources();
	void setGpuProgram(ResourceId resId);
	void setGpuProgramRenderMethod(const char* pName);
	const String& gpuProgramFileName() const;
	ResourceId gpuProgram() const;
	GpuProgramRenderMethod* gpuProgramRenderMethod() const;
	const Array<TextureSamplerInfo>& textureSamplerInfo() const;
	const Array<RenderState>& renderStates() const;
	void commitRenderStates();

protected:
	void updateRenderStateObjects();

	ResourceId m_gpuProgram;
	String m_gpuProgramRenderMethod;
	GpuProgram* m_pGpuProgram;
	GpuProgramRenderMethod* m_pGpuProgramRenderMethod;
	Array<TextureSamplerInfo> m_textureSamplerInfo;
	Array<RenderState> m_renderStates;
	RasterizerState m_rasterizerState;
	BlendState m_blendState;
	DepthStencilState m_depthStencilState;
};

//! This class is a high level surface material, which can be set to sub-meshes of a mesh
class E_API Material : public Resource, public Naming
{
public:
	static const int kFileVersion = 1;
	B_RUNTIME_CLASS;
	Material();
	virtual ~Material();

	//! load the resource, if pFilename is nullptr, will use the m_fileName,
	//! else will set m_fileName to pFilename and use that
	bool load(Stream* pStream);
	//! unload the resource
	bool unload();
	void onDependencyUnload(Resource* pDependencyRes);

	const Array<MaterialLayer*>& layers() const;
	//! \return the layer by its name, or nullptr if not found
	MaterialLayer* findLayer(const char* pName) const;
	//! free the used resources
	void free();
		
protected:
	Array<MaterialLayer*> m_layers;
};
}