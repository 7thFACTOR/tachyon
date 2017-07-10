#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/class_registry.h"
#include "base/table.h"
#include "base/math/vec3.h"
#include "physics/joint.h"
#include "physics/force_field.h"

namespace engine
{
class PhysicsBody;
class PhysicsSoftBody;
class PhysicsJoint;
class PhysicsShape;
class PhysicsCharacter;
class PhysicsCloth;
class PhysicsMaterial;
class PhysicsForceField;
class PhysicsRagDoll;
class PhysicsRope;
class PhysicsVehicle;

//! This class is a contact pair info between two patches of two bodies
struct PhysicsContactPairInfo
{
	Vec3 normal;
	Array<Vec3> points;
};

//! This class is used for user defined physics events handling (ex.: make a sound when a joint broke)
class E_API PhysicsEventListener
{
public:
	//! called by the provider when a contact took place between two bodies
	virtual bool onContact(PhysicsBody* body1, PhysicsBody* body2, const Array<PhysicsContactPairInfo>& contacts);
	//! called when a joint broke (if the joint is set as breakable)
	virtual bool onJointBreak(f32 breakImpulse, PhysicsJoint* joint);
	//! called when the bodies were enabled, by the user or automatically when hit
	virtual bool onEnableBodies(const Array<PhysicsBody*>& bodies);
	//! called when the bodies were disabled, by the user or automatically when resting/sleeping started
	virtual bool onDisableBodies(const Array<PhysicsBody*>& bodies);
	//! called by 3rdparty user made physics plug-in, maybe other physics API have more events to report
	virtual bool onUserEvent(u32 eventType, void* data);
};

//! A physics island contains a group of physic bodies which can interact with each other
class E_API PhysicsSpace
{
public:
	PhysicsSpace();
	virtual ~PhysicsSpace();

	virtual bool create() = 0;
	//! set the physics events handler interface,
	//! whose methods will be called when specific events occur in the simulation
	virtual void setListener(PhysicsEventListener* handler);
	//! set the number of maximum allowed body count in the simulation of this physics space
	virtual void setMaxBodyCount(u32 count);
	//! set the global gravity vector
	virtual void setGravity(const Vec3& gravity);

	//! \return the physics events handler
	PhysicsEventListener* getListener() const;
	//! \return the reference to the array of physics bodies in this physics space
	const Array<PhysicsBody*>& getBodies() const;
	//! \return the maximum number of bodies allowed in this space's simulation
	u32 getMaxBodyCount() const;
	void removeBody(PhysicsBody* body);
	//! \return the global gravity vector/direction
	const Vec3& getGravity() const;

	//! add a new physics body
	virtual PhysicsBody* createBody() = 0;
	//! add a new physics body
	virtual PhysicsCharacter* createCharacter() = 0;
	//! add a new physics material
	virtual PhysicsMaterial* createMaterial() = 0;
	//! add a new joint to the physics space
	virtual PhysicsJoint* createJoint(PhysicsJointType type) = 0;
	//! add a new force field
	virtual PhysicsForceField* createForceField(PhysicsForceFieldType type) = 0;
	//! add a new cloth
	virtual PhysicsCloth* createCloth() = 0;
	//! add a new rag-doll
	virtual PhysicsRagDoll* createRagDoll() = 0;
	//! add a new rope
	virtual PhysicsRope* createRope() = 0;
	//! add a new soft body
	virtual PhysicsSoftBody* createSoftBody() = 0;
	//! add a new physics vehicle
	virtual PhysicsVehicle* createVehicle() = 0;
	//! create a new box shape, you can cast this ptr to IBoxPhysicsShape to get more methods specific to the shape
	virtual PhysicsShape* createBoxShape(f32 width, f32 height, f32 depth) = 0;
	//! create a new cylinder shape, you can cast this ptr to ICylinderPhysicsShape to get more methods specific to the shape
	virtual PhysicsShape* createCylinderShape(f32 radius, f32 height, size_t segments) = 0;
	//! create a new capsule shape, you can cast this ptr to ICapsulePhysicsShape to get more methods specific to the shape
	virtual PhysicsShape* createCapsuleShape(f32 radius, f32 height) = 0;
	//! create a new sphere shape, you can cast this ptr to ISpherePhysicsShape to get more methods specific to the shape
	virtual PhysicsShape* createSphereShape(f32 radius) = 0;
	//! create a new plane shape, you can cast this ptr to IPlanePhysicsShape to get more methods specific to the shape
	virtual PhysicsShape* createPlaneShape(const Vec3& planeNormal, f32 distanceAlongNormal) = 0;
	//! create a new convex hull shape, you can cast this ptr to IConvexHullPhysicsShape to get more methods specific to the shape
	virtual PhysicsShape* createConvexHullShape(const Array<Vec3>& pointCloud) = 0;
	//! create a new triangle mesh shape, you can cast this ptr to ITriangleMeshPhysicsShape to get more methods specific to the shape
	virtual PhysicsShape* createTriangleMeshShape(Mesh* mesh) = 0;
	//! create a new height field shape, you can cast this ptr to IHeightFieldPhysicsShape to get more methods specific to the shape
	virtual PhysicsShape* createHeightFieldShape(const TableF32& heightMap) = 0;

protected:
	//! physics bodies in the simulation of this space
	Array<PhysicsBody*> bodies;
	//! the space's gravity vector and size
	Vec3 gravity;
	//! the physics events handler interface
	PhysicsEventListener* physicsEventListener;
	//! maximum bodies allowed in the simulation
	size_t maxBodyCount;
};

}