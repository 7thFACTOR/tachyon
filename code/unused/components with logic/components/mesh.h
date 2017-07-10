#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "render/frustum.h"
#include "render/resources/mesh_resource.h"

namespace engine
{
//! A camera scene object, is the camera through where all scene is viewed
struct MeshComponent : Component
{
	enum
	{
		TypeId = ComponentType::Mesh,
		DefaultCount = kMaxComponentCountPerType
	};

	MeshComponent()
		: mesh(kInvalidResourceId)
	{
	}

	ResourceId mesh;
};
}