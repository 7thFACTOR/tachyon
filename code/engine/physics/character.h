#pragma once
#include "base/defines.h"
#include "base/class_registry.h"
#include "base/math/vec3.h"
#include "core/defines.h"

namespace engine
{
using namespace base;

//! A physics object which has special behavior to be used in controlling avatars (ex: first person shooter player body)
class E_API PhysicsCharacter
{
public:
	virtual ~PhysicsCharacter();
	virtual bool create(f32 capsuleRadius, f32 capsuleHeight) = 0;
	virtual void moveCharacter(const Vec3& delta, f32 minDistance, f32 autoStepSharpness) = 0;
	virtual void enableCollisions(bool enable) = 0;
	virtual Vec3 getTranslation() const = 0;
	virtual void setTranslation(const Vec3& pos) = 0;
	virtual void setCapsuleHeight(f32 height) = 0;
	virtual void setCapsuleRadius(f32 radius) = 0;
	virtual f32 getCapsuleHeight() const = 0;
	virtual f32 getCapsuleRadius() const = 0;
	virtual bool isCapsuleWorldOverlapping(f32 radius, f32 height) const = 0;
	virtual bool checkGroundIntersection(
			f32* outDist,
			Vec3* outWorldPt,
			Vec3* outWorldNormal) const = 0;
	virtual bool isCollidingSides() const = 0;
	virtual bool isCollidingGround() const = 0;
	virtual bool isCollidingUp() const = 0;

protected:
	//! this class can only be created through PhysicsSpace
	PhysicsCharacter();
};

}