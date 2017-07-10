#pragma once
//*****************************************************************************
// Nytro Game Engine
//
// (C) Copyright 7thFACTOR Entertainment
// All rights reserved
//*****************************************************************************
#include "system/array.h"
#include "system/string.h"
#include "system/map.h"
#include "system/defines.h"
#include "system/resource.h"
#include "physics/physics_body.h"
#include "physics/physics_joint.h"
#include "physics/physics_shape.h"
#include "math/vec3.h"
#include "math/quat.h"

namespace nytro
{
class N_API PhysicsSetupData : public Resource
{
public:
	struct ShapeInfo
	{
		f32 m_mass, m_density;
		Vec3 m_translation;
		Quat m_rotation;
		PhysicsShape::EType m_type;
		Map<String, String> m_properties;
	};

	struct BodyInfo
	{
		String m_name;
		f32 m_mass, m_density;
		PhysicsBody::EBehavior m_behavior;
		Vec3 m_translation;
		Quat m_rotation;
		bool m_bEnabled, m_bEnableGravity;
		Array<ShapeInfo> m_shapes;
	};

	struct JointInfo
	{
		PhysicsJoint::EType m_type;
		String m_bodyName[2];
		bool m_bBreakable, m_bCollideBodies, m_bMotorized, m_bSpring;
		f32 m_breakForce, m_breakTorque;
		Vec3 m_localAxis;
		f32 m_motorForce, m_motorVelocity;
		f32 m_spring, m_springDamper, m_springRestingValue;
		Vec3 m_localBodyAnchor[2];
		Vec3 m_translation;
		Quat m_rotation;
		//TODO: limits
	};

	PhysicsSetupData();
	virtual ~PhysicsSetupData();

	bool load(const char* pFilename = nullptr);
	bool unload();
	void createInstances(
		Array<PhysicsBody*>* pArrBodies /*out*/,
		Array<PhysicsJoint*>* pArrJoints /*out*/,
		GeometryBuffer* pGbTriangleMesh = nullptr,
		Array<Vec3>* pConvexPointCloud = nullptr);
	const Array<BodyInfo>& bodies() const;
	const Array<JointInfo>& joints() const;

protected:
	static PhysicsBody* findBodyByName(const Array<PhysicsBody*>& rBodies, const char* pName);

	Array<BodyInfo> m_bodies;
	Array<JointInfo> m_joints;
};
}