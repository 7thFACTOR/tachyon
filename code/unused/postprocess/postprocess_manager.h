#pragma once
#include "base/string.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/util.h"
#include "base/variant.h"
#include "render/renderer.h"
#include "render/resources/texture_resource.h"
#include "base/serialize.h"

namespace engine
{
class SceneRenderer;

//! A texture map used in rendering post process, can be for example
//! the depth buffer map, or the shadow map, scene map, HDR/LDR blur maps etc.
class E_API PostProcessMap
{
public:
	PostProcessMap();
	virtual ~PostProcessMap();

	virtual bool create();
	virtual void free();

	//! the map's name
	String name;
	//! the map's scaling factor with respect to the scene map/screen size
	f32 scale;
	//! do scale map when resolution changes ?
	bool bAutoScale;
	//! has a color map attached ?
	bool bColorMap;
	//! has a depth map attached ?
	bool bDepthMap;
	//! has a stencil map attached ?
	bool bStencilMap;
	//! the width in pixels ( 0 - will take the size of the scene map width )
	u32 width;
	//! the height in pixels ( 0 - will take the size of the scene map height )
	u32 height;
	//! the texture map object
	ResourceId texture;
	TextureFormat format;
	//! the id given from registering as a system texture
	u32 sysTexId;
	//! this map can be a texture from a file on disk, if empty then this map
	//! will be created as render target
	String fileName;
};

//! The render pipeline stage object holds the commands executed in this render
//! stage
class E_API PostProcess : public ClassInfo
{
public:
	PostProcess();
	virtual ~PostProcess();

	virtual void process() {}
	const Map<String, PostProcessMap*>& inputMaps() const;
	const Map<String, PostProcessMap*>& outputMaps() const;
	void setActive(bool bActive);
	bool isActive() const;
	void setSortKey(u32 key);
	u32 sortKey() const;
	virtual void free();
	const String& name() const { return m_name; }
	void setName(const String& name) { m_name = name; }

protected:
	//! is stage active
	String m_name;
	bool m_bActive;
	u32 m_sortKey;
	Map<String, PostProcessMap*> m_inputMaps;
	Map<String, PostProcessMap*> m_outputMaps;
};

//! The render pipeline tells the scene manager or other rendering entity how
//! the scene will be rendered, the steps to follow
class E_API PostProcessManager
{
public:
	PostProcessManager();
	virtual ~PostProcessManager();

	//! \return the map with the specified name
	PostProcessMap* map(const String& name) const;
	//! \return the map with the specified name
	PostProcess* stage(const String& name) const;
	//! \return the maps array
	const Array<PostProcessMap*>& maps() const;
	//! \return the maps aliases array, used to store references to maps, toggle
	//! rendering from one map to another
	const Map<String, PostProcessMap*>& mapAliases() const;
	void setMapAlias(const String& name, PostProcessMap* pMap);
	const Array<PostProcess*>& postProcesses() const;
	void registerMaps();
	void unregisterMaps();
	void recreateMaps();
	bool addMap(PostProcessMap* pMap);
	bool addPostProcess(PostProcess* pPostProcess);
	void removeMaps();
	void removePostProcesses();
	void deleteMaps();
	void deletePostProcesses();
	void removeMap(PostProcessMap* pMap);
	void removePostProcess(PostProcess* pPostProcess);
	void process();
	virtual void free();

protected:
	//! the maps to use in this pipeline
	Array<PostProcessMap*> m_maps;
	//! the render stages
	Array<PostProcess*> m_postProcesses;
	//! map aliases map, used to reference previous used maps in previous stages
	//! not by name but by alias name
	Map<String, PostProcessMap*> m_mapAliases;
};

}