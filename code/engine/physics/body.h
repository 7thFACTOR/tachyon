#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "core/resource.h"

namespace engine
{
class PhysicsSpace;
class PhysicsShape;
class PhysicsMaterial;

enum class PhysicsBodyBehavior
{
	//! a simple dynamic body (ex: a basketball)
	Dynamic,
	//! a dynamic user controlled/positioned body
	Kinematic,
	//! a static body (ex: walls or platforms)
	Static
};

//! This class represents a physics body, composed of one or many physic shapes (sphere,mesh,cylinder,box, etc.)
class E_API PhysicsBody
{
public:
	virtual ~PhysicsBody();

	virtual bool create() = 0;
	//! \return the body behavior type
	PhysicsBodyBehavior getBehavior() const;
	//! \return the physics space where this body is living in
	PhysicsSpace* getPhysicsSpace() const;
	//! true if gravity enabled for this body
	virtual bool isGravityEnabled() const;
	//! true if this body is enabled and its not sleeping, it is moving
	virtual bool isSleeping() const;
	//! \return auto sleep time for this body
	virtual f32 getAutoSleepTime() const;
	//! \return true if auto sleep is enabled
	virtual bool isAutoSleeping() const;
	//! \return the linear velocity of this body
	virtual Vec3 getLinearVelocity() const;
	//! \return the angular velocity of this body
	virtual Vec3 getAngularVelocity() const;
	//! \return the total mass of this body
	f32 getMass() const;
	//! enable or disable this body
	virtual void wakeUp(bool value);
	//! enable or disable gravity for this body
	virtual void setEnableGravity(bool value);
	//! set the body behavior type
	virtual void setBehavior(PhysicsBodyBehavior value);
	//! set the auto disable time
	virtual void setAutoSleepTime(f32 value);
	//! set auto disable on/off
	virtual void setAutoSleep(bool value);
	//! set the mass of this body
	virtual void setMass(f32 mass);
	//! add a new force to the body
	virtual void addForce(const Vec3& value);
	//! add a new torque to the body
	virtual void addTorque(const Vec3& value);
	//! add a force relative to this body
	virtual void addRelativeForce(const Vec3& value);
	//! add a torque relative to this body
	virtual void addRelativeTorque(const Vec3& value);
	//! add force at global position
	virtual void addForceAtPosition(const Vec3& force, const Vec3& pos);
	//! add force at relative(local) position in the body's space
	virtual void addForceAtRelativePosition(const Vec3& force, const Vec3& pos);
	//! add relative (local) force at global position
	virtual void addRelativeForceAtPosition(const Vec3& force, const Vec3& pos);
	//! add relative (local) force at relative position in body's space
	virtual void addRelativeForceAtRelativePosition(const Vec3& force, const Vec3& pos);
	//! set linear velocity of the body
	virtual void setLinearVelocity(f32 x, f32 y, f32 z);
	//! set linear velocity of the body
	virtual void setLinearVelocity(const Vec3& value);
	//! set angular velocity of the body
	virtual void setAngularVelocity(f32 x, f32 y, f32 z);
	//! set angular velocity of the body
	virtual void setAngularVelocity(const Vec3& value);
	//! set the physics space associated with this body
	virtual void setPhysicsSpace(PhysicsSpace* space);
	//! get position and rotation
	virtual void setTransform(const Vec3& pos, const Quat& quat) = 0;
	//! get position and rotation
	virtual void setTranslation(const Vec3& pos) = 0;
	//! get position and rotation
	virtual void setRotation(const Quat& quat) = 0;
	//! get position and rotation
	virtual void getTransform(Vec3& pos, Quat& quat) = 0;
	//! add a shape to this body
	virtual void addShape(PhysicsShape* shape);
	//! remove a shape from this body
	virtual void removeShape(PhysicsShape* shape);

protected:
	//! the body's behavior type
	PhysicsBodyBehavior behavior;
	//! true if body is influenced by gravity
	bool useGravity;
	//! true if body is enabled
	bool sleeping;
	//! true if body has auto disable enabled
	bool autoSleep;
	//! body auto disable time in milliseconds
	f32 autoSleepTime;
	//! the physics space of this body
	PhysicsSpace* physicsSpace;
	//! the shapes array of this body
	Array<PhysicsShape*> shapes;
	//! total body mass
	f32 mass;
};

}