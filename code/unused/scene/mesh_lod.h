#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/types.h"
#include "core/defines.h"

namespace engine
{
using namespace base;
class Mesh;
class Material;
struct MeshPart;
struct MeshCluster;

//! A mesh level of detail (LOD)
struct MeshLOD
{
	//! the distance from where this LOD mesh is being made the current visible mesh of the model
	f32 fromDistance;
	//! the LOD's mesh
	ResourceId mesh;
	//! the mesh parts materials
	//TMeshClusterToMaterialMap clusterMaterials;
};

}