// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "physics/joint.h"
#include "base/assert.h"

namespace engine
{
PhysicsJoint::PhysicsJoint()
{
	body1 = nullptr;
	body2 = nullptr;
	breakable = false;
	collisionBetweenBodies = true;
	motorized = false;
	limitPointOnBody2 = false;
	useLimits = false;
	maxBreakForce = 1;
	maxBreakTorque = 1;
	maxMotorForce = 1;
	motorVelocity = 1;
	physicsSpace = nullptr;
}

PhysicsJoint::~PhysicsJoint()
{}

bool PhysicsJoint::create()
{
	return false;
}

void PhysicsJoint::linkBodies(PhysicsBody* newBody1, PhysicsBody* newBody2)
{
	B_ASSERT(newBody1 && newBody2);
	body1 = newBody1;
	body2 = newBody2;
}

void PhysicsJoint::setBreakable(bool newBreakable, f32 newMaxBreakForce, f32 newMaxBreakTorque)
{
	breakable = newBreakable;
	maxBreakForce = newMaxBreakForce;
	maxBreakTorque = newMaxBreakTorque;
}

void PhysicsJoint::setCollisionBetweenBodies(bool collide)
{
	collisionBetweenBodies = collide;
}

void PhysicsJoint::setLocalAxis(const Vec3& axis)
{
	localAxis = axis;
}

void PhysicsJoint::setMotor(bool newMotorized, f32 newMaxForce, f32 newVelocity)
{
	motorized = newMotorized;
	maxMotorForce = newMaxForce;
	motorVelocity = newVelocity;
}

void PhysicsJoint::addLimitPlane(const Vec3& planeNormal, const Vec3& planePoint, f32 restitution)
{}

void PhysicsJoint::setLimitPoint(const Vec3& point, bool isOnBody2)
{
	limitPoint = point;
	limitPointOnBody2 = isOnBody2;
}

void PhysicsJoint::clearLimitPlanes()
{}

void PhysicsJoint::setSpring(bool springEnabled, f32 springValue, f32 damperValue, f32 restingValue)
{
	spring.enabled = springEnabled;
	spring.spring = springValue;
	spring.damper = damperValue;
	spring.targetValue = restingValue;
}

void PhysicsJoint::setLocalBodyAnchor(u32 index, const Vec3& point)
{
	B_ASSERT(B_INBOUNDS(index, 0, 2));
	localBodyAnchor[index] = point;
}

void PhysicsJoint::setPhysicsSpace(PhysicsSpace* space)
{
	B_ASSERT(space);
	physicsSpace = space;
}

void PhysicsJoint::setLimits(bool enabled, PhysicsJointLimit* low, PhysicsJointLimit* high)
{
	useLimits = enabled;

	if (low)
	{
		lowLimit = *low;
	}
	
	if (high)
	{
		highLimit = *high;
	}
}

//TODO: put these inline 
bool PhysicsJoint::isBreakable() const
{
	return breakable;
}

f32 PhysicsJoint::getMaxBreakForce() const
{
	return maxBreakForce;
}

f32 PhysicsJoint::getMaxBreakTorque() const
{
	return maxBreakTorque;
}

bool PhysicsJoint::isCollisionBetweenBodies() const
{
	return collisionBetweenBodies;
}

PhysicsBody* PhysicsJoint::getLinkedBody1() const
{
	return body1;
}

PhysicsBody* PhysicsJoint::getLinkedBody2() const
{
	return body2;
}

const Vec3& PhysicsJoint::getLocalAxis() const
{
	return localAxis;
}

PhysicsSpace* PhysicsJoint::getPhysicsSpace() const
{
	return physicsSpace;
}

bool PhysicsJoint::isMotorized() const
{
	return motorized;
}

f32 PhysicsJoint::getMotorMaxForce() const
{
	return maxMotorForce;
}

f32 PhysicsJoint::getMotorVelocity() const
{
	return motorVelocity;
}

bool PhysicsJoint::hasLimits() const
{
	return useLimits;
}

void PhysicsJoint::setTranslation(const Vec3& value)
{
	translation = value;
}

void PhysicsJoint::setRotation(const Quat& value)
{
	rotation = value;
}

}