#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "core/resource.h"

namespace engine
{
class PhysicsSpace;
class PhysicsBody;
class PhysicsShape;

//! A spring info holder, used for various physics usages
struct PhysicsSpring
{
	//! is this spring enabled ?
	bool enabled = false;
	//! spring force
	f32 spring = 0.0f;
	//! dampening force
	f32 damper = 0.0f;
	//! target value when the spring is resting
	f32 targetValue = 0.0f;
};

//! This class represents a physics material, with several properties, used to be assigned to physics bodies, for the collision response and forces applied
//TODO: make a proper POD resource struct
class E_API PhysicsMaterial : public Resource
{
public:
	PhysicsMaterial();
	virtual ~PhysicsMaterial();

	//! load the resource, if pFilename is nullptr, will use the m_fileName,
	//! else will set pFilename as m_fileName and use that
	bool load(const String& filename);
	//! unload the resource
	bool unload();
	//! update the physics material after setting some of the class' members
	virtual void update();

protected:
	f32 dynamicFriction, staticFriction, restitution;
	bool disableFriction, solid;
	PhysicsSpring spring;
};

}