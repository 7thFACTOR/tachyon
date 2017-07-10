#include "physics/body.h"
#include "core/globals.h"
#include "base/assert.h"

namespace engine
{
PhysicsBody::~PhysicsBody()
{}

PhysicsBodyBehavior PhysicsBody::getBehavior() const
{
	return behavior;
}

bool PhysicsBody::isGravityEnabled() const
{
	return useGravity;
}

bool PhysicsBody::isSleeping() const
{
	return sleeping;
}

f32 PhysicsBody::getAutoSleepTime() const
{
	return autoSleepTime;
}

bool PhysicsBody::isAutoSleeping() const
{
	return autoSleep;
}

f32 PhysicsBody::getMass() const
{
	return mass;
}

void PhysicsBody::wakeUp(bool value)
{
	sleeping = value;
}

void PhysicsBody::setEnableGravity(bool value)
{
	useGravity = value;
}

void PhysicsBody::setBehavior(PhysicsBodyBehavior value)
{
	behavior = value;
}

void PhysicsBody::setAutoSleepTime(f32 value)
{
	autoSleepTime = value;
}

void PhysicsBody::setAutoSleep(bool value)
{
	autoSleep = value;
}

Vec3 PhysicsBody::getLinearVelocity() const
{
	return Vec3();
}

Vec3 PhysicsBody::getAngularVelocity() const
{
	return Vec3();
}

void PhysicsBody::addForce(const Vec3& value)
{
}

void PhysicsBody::addTorque(const Vec3& value)
{
}

void PhysicsBody::addRelativeForce(const Vec3& value)
{
}

void PhysicsBody::addRelativeTorque(const Vec3& value)
{
}

void PhysicsBody::addForceAtPosition(const Vec3& force, const Vec3& pos)
{
}

void PhysicsBody::addForceAtRelativePosition(const Vec3& force, const Vec3& pos)
{
}

void PhysicsBody::addRelativeForceAtPosition(const Vec3& force, const Vec3& pos)
{
}

void PhysicsBody::addRelativeForceAtRelativePosition(const Vec3& force, const Vec3& pos)
{
}

void PhysicsBody::setLinearVelocity(f32 x, f32 y, f32 z)
{
}

void PhysicsBody::setLinearVelocity(const Vec3& value)
{
	setLinearVelocity(value.x, value.y, value.z);
}

void PhysicsBody::setAngularVelocity(f32 x, f32 y, f32 z)
{
}

void PhysicsBody::setAngularVelocity(const Vec3& value)
{
	setAngularVelocity(value.x, value.y, value.z);
}

void PhysicsBody::setPhysicsSpace(PhysicsSpace* space)
{
	B_ASSERT(space);
	physicsSpace = space;
}

PhysicsSpace* PhysicsBody::getPhysicsSpace() const
{
	return physicsSpace;
}

void PhysicsBody::addShape(PhysicsShape* shape)
{
	B_ASSERT(shape);
	shapes.append(shape);
}

void PhysicsBody::removeShape(PhysicsShape* shape)
{
	Array<PhysicsShape*>::Iterator iter = shapes.find(shape);

	if (iter != shapes.end())
	{
		shapes.erase(iter);
	}
}

void PhysicsBody::setMass(f32 newMass)
{
	mass = newMass;
}

}