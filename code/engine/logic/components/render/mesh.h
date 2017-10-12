// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "graphics/frustum.h"
#include "core/resources/mesh_resource.h"

namespace engine
{
class Mesh;
struct MeshResource;

struct MeshComponent : Component
{
	static const ComponentTypeId typeId = StdComponentTypeId_Mesh;

	MeshComponent() {}

	ResourceId meshResourceId = nullResourceId;
	bool isStatic = false;
	Mesh* renderMesh = nullptr;
	MeshResource* meshResource = nullptr;
};

}