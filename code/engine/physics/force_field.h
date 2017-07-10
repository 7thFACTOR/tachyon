#pragma once
#include "base/defines.h"
#include "base/class_registry.h"
#include "logic/components/render/transform.h"
#include "base/math/bbox.h"

namespace engine
{
class Mesh;

enum class PhysicsForceFieldType
{
	Sphere,
	Capsule,
	Box,
	Hull
};

class E_API PhysicsForceField
{
public:
	virtual ~PhysicsForceField();

protected:
	//! create only through PhysicsSpace
	PhysicsForceField();

	PhysicsForceFieldType type;
	f32 forceRadius, forceHeight;
	BBox forceBox;
	Mesh* hull = nullptr;
};

}