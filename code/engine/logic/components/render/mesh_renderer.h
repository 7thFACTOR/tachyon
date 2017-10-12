// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "graphics/frustum.h"
#include "core/resources/mesh_resource.h"
#include "logic/component.h"

namespace engine
{
struct MeshRendererComponent : Component
{
	static const ComponentTypeId typeId = StdComponentTypeId_MeshRenderer;

	MeshRendererComponent() {}

	Array<ResourceId> materials;
	bool castShadows = true;
	bool receiveShadows = true;
	// cache
	Array<class MaterialResource*> materialResources;
	Array<class GpuProgram*> materialGpuPrograms;
	Array<struct GpuProgramResource*> materialGpuProgramResources;
};

}