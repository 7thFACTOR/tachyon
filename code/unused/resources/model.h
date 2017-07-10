#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/resource.h"

namespace engine
{
class Animation;
class Mesh;
class Material;
class MeshLevelOfDetail;

enum EModelChunkIds
{
	eModelChunk_Header,
	eModelChunk_Meshes,
	eModelChunk_LODs
};

//! Model resource data has info on submesh gpu program and textures
class E_API ModelData : public Resource
{
public:
	//! map of mesh name and mesh file data
	typedef Map<String, ResourceId> TMeshNameMeshDataMap;
	B_RUNTIME_CLASS;

	ModelData();
	virtual ~ModelData();

	//! load the model data
	bool load(Stream* pStream);
	//! unload the model data
	bool unload();
	//! \return the submeshes settings
	const Array<MeshLevelOfDetail*>& levelsOfDetail() const;

protected:
	//! the mesh levels of detail
	Array<MeshLevelOfDetail*> m_levelsOfDetail;
	TMeshNameMeshDataMap m_usedMeshes;
	bool m_bMustReload;
};
}