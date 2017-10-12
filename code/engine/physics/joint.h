// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/class_registry.h"
#include "logic/components/render/transform.h"
#include "base/math/vec3.h"
#include "physics/physics_material.h"

namespace engine
{
class PhysicsSpace;
class PhysicsBody;

//! The joint type
enum class PhysicsJointType
{
	Undefined,
	Cylindrical,
	Universal,
	Distance,
	Fixed,
	PointInPlane,
	PointOnLine,
	Prismatic,
	Pulley,
	Revolute,
	Spherical,
	
	Count
};

//! This class is used for physics joints limits
struct PhysicsJointLimit
{
	f32 value = 0.0f;
	f32 restitution = 0.0f;
	f32 hardness = 0.0f;
};

//! A physics joint, can join together two physics bodies
class E_API PhysicsJoint
{
public:
	PhysicsJoint();
	virtual ~PhysicsJoint();

	virtual bool create();
	//! join/link two physics bodies together using this joint
	virtual void linkBodies(PhysicsBody* body1, PhysicsBody* body2);
	//! set this joint as breakable or not
	virtual void setBreakable(bool breakable, f32 maxBreakForce, f32 maxBreakTorque);
	//! set to true if collision between the two bodies of the joint must be performed
	virtual void setCollisionBetweenBodies(bool collide);
	//! set joint's local axis
	virtual void setLocalAxis(const Vec3& axis);
	//! set this joint to be motorized and maintain a velocity within a max force (only pulley and revolute joints can be motorized)
	virtual void setMotor(bool motorized, f32 maxForce, f32 velocity);
	//! set physics space
	virtual void setPhysicsSpace(PhysicsSpace* space);
	//! add a joint limiting plane
	virtual void addLimitPlane(const Vec3& planeNormal, const Vec3& planePoint, f32 restitution);
	//! set the limit point for joint limiting
	virtual void setLimitPoint(const Vec3& point, bool isOnBody2);
	//! clear all limit planes
	virtual void clearLimitPlanes();
	//! set joint to be springy
	virtual void setSpring(bool enabled, f32 spring, f32 damper, f32 restingValue);
	//! set local body anchor point
	virtual void setLocalBodyAnchor(u32 index, const Vec3& point);
	//! set joint limits
	virtual void setLimits(bool enabled, PhysicsJointLimit* low, PhysicsJointLimit* high);
	//! \return true if this joint is breakable
	bool isBreakable() const;
	//! \return max break force this joint can withstand if this joint is breakable
	f32 getMaxBreakForce() const;
	//! \return max break force this joint can withstand if this joint is breakable
	f32 getMaxBreakTorque() const;
	//! \return true if collision between the two bodies of the joint must be performed
	bool isCollisionBetweenBodies() const;
	//! \return the connected body 1
	PhysicsBody* getLinkedBody1() const;
	//! \return the connected body 2
	PhysicsBody* getLinkedBody2() const;
	//! \return joint's local axis
	const Vec3& getLocalAxis() const;
	//! \return the physics space this joint lives in
	PhysicsSpace* getPhysicsSpace() const;
	//! \return true if this joint is motorized
	bool isMotorized() const;
	//! \return motor max force
	f32 getMotorMaxForce() const;
	//! \return motor velocity
	f32 getMotorVelocity() const;
	//! \return true if limits on
	bool hasLimits() const;
	void setTranslation(const Vec3& value);
	void setRotation(const Quat& value);

protected:
	PhysicsJointType type;
	//! first linked body to this joint
	PhysicsBody* body1;
	//! second linked body to this joint
	PhysicsBody* body2;
	//! is this joint breakable
	bool breakable;
	//! is there collision between the bodies attached to this joint
	bool collisionBetweenBodies;
	//! is this joint motorized
	bool motorized;
	//! is limit point only on second body
	bool limitPointOnBody2;
	//! joint has limits
	bool useLimits;
	//! maximum break force on breakable joints
	f32 maxBreakForce;
	//! maximum break torque
	f32 maxBreakTorque;
	//! maximum motor force
	f32 maxMotorForce;
	//! motor's velocity
	f32 motorVelocity;
	//! joint's local axis
	Vec3 localAxis;
	//! joint body limit point
	Vec3 limitPoint;
	//! the physics space this joint is in
	PhysicsSpace* physicsSpace;
	//! joint's spring settings
	PhysicsSpring spring;
	//! low limit
	PhysicsJointLimit lowLimit;
	//! high limit
	PhysicsJointLimit highLimit;
	//! local body anchors, used in some joint types
	Vec3 localBodyAnchor[2];
	Vec3 translation;
	Quat rotation;
};

}