#include "physics/space.h"
#include "physics/body.h"
#include "physics/joint.h"
#include "physics/cloth.h"
#include "physics/force_field.h"
#include "physics/ragdoll.h"
#include "physics/rope.h"
#include "physics/soft_body.h"
#include "physics/vehicle.h"
#include "physics/shape.h"
#include "physics/character.h"

namespace engine
{
PhysicsSpace::PhysicsSpace()
{
	gravity.set(0, -9.81f, 0);
	physicsEventListener = nullptr;
	maxBodyCount = 5000; //TODO: ?!? :D
}

PhysicsSpace::~PhysicsSpace()
{}

void PhysicsSpace::setListener(PhysicsEventListener* handler)
{
	physicsEventListener = handler;
}

PhysicsEventListener* PhysicsSpace::getListener() const
{
	return physicsEventListener;
}

const Array<PhysicsBody*>& PhysicsSpace::getBodies() const
{
	return bodies;
}

void PhysicsSpace::setMaxBodyCount(u32 count)
{
	maxBodyCount = count;
}

u32 PhysicsSpace::getMaxBodyCount() const
{
	return maxBodyCount;
}

void PhysicsSpace::removeBody(PhysicsBody* body)
{
	Array<PhysicsBody*>::Iterator iter = bodies.find(body);
	
	if (iter != bodies.end())
	{
		bodies.erase(iter);
	}
}

void PhysicsSpace::setGravity(const Vec3& newGravity)
{
	gravity = newGravity;
}

const Vec3& PhysicsSpace::getGravity() const
{
	return gravity;
}

//---

bool PhysicsEventListener::onContact(
	PhysicsBody* body1,
	PhysicsBody* body2,
	const Array<PhysicsContactPairInfo>& contacts)
{
	return false;
}

bool PhysicsEventListener::onJointBreak(f32 breakImpulse, PhysicsJoint* joint)
{
	return false;
}

bool PhysicsEventListener::onEnableBodies(const Array<PhysicsBody*>& bodies)
{
	return false;
}

bool PhysicsEventListener::onDisableBodies(const Array<PhysicsBody*>& bodies)
{
	return false;
}

bool PhysicsEventListener::onUserEvent(u32 eventType, void* data)
{
	return false;
}

}