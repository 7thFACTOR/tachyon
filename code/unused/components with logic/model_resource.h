#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/resource.h"

namespace engine
{
namespace ModelChunkTypes
{
	enum ModelChunkType
	{
		Header,
		LevelsOfDetail
	};
}
typedef ModelChunkTypes::ModelChunkType ModelChunkType;

struct MeshLevelOfDetail
{
	//! the distance from where this LOD mesh is being made the current visible mesh of the model
	f32 lodDistance;
	ResourceId mesh;
	u32 clusterMaterialCount;
	ResourceId* clusterMaterials;
};

struct ModelResource
{
	//! the mesh levels of detail
	u32 levelOfDetailCount;
	MeshLevelOfDetail* levelsOfDetail;
};

}