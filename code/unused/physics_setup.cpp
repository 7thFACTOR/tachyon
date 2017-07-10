#include <stdlib.h>
#include "physics/physics_setup_data.h"
#include "physics/physics_manager.h"
#include "physics/physics_space.h"
#include "system/core.h"
#include "system/util.h"

namespace nytro
{
PhysicsSetupData::PhysicsSetupData()
{
}

PhysicsSetupData::~PhysicsSetupData()
{
}

bool PhysicsSetupData::load(const char* pFilename)
{
	//Resource::load(pFilename);
	//XmlDocument xml;

	//unload();

	//if (!xml.load(m_fileName.c_str()))
	//{
	//	N_LOG_ERROR("Could not load physics setup file '%s'", m_fileName.c_str());

	//	return false;
	//}

	//XmlElement* pBodiesElem = xml.getRoot().getChild("bodies");
	//XmlElement* pJointsElem = xml.getRoot().getChild("joints");

	//Map<String, PhysicsBody::EBehavior> bodyBehaviors;

	//bodyBehaviors["static"] = PhysicsBody::eBehavior_Static;
	//bodyBehaviors["controlled"] = PhysicsBody::eBehavior_Kinematic;
	//bodyBehaviors["dynamic"] = PhysicsBody::eBehavior_Dynamic;

	//Map<String, PhysicsShape::EType> shapeTypes;

	//shapeTypes["box"] = PhysicsShape::eType_Box;
	//shapeTypes["cylinder"] = PhysicsShape::eType_Cylinder;
	//shapeTypes["capsule"] = PhysicsShape::eType_Capsule;
	//shapeTypes["sphere"] = PhysicsShape::eType_Sphere;
	//shapeTypes["plane"] = PhysicsShape::eType_Plane;
	//shapeTypes["convexHull"] = PhysicsShape::eType_ConvexHull;
	//shapeTypes["triangleMesh"] = PhysicsShape::eType_TriangleMesh;
	//shapeTypes["heightField"] = PhysicsShape::eType_HeightField;

	//Map<String, PhysicsJoint::EType> jointTypes;

	//jointTypes["undefined"] = PhysicsJoint::eType_Undefined;
	//jointTypes["cylindrical"] = PhysicsJoint::eType_Cylindrical;
	//jointTypes["universal"] = PhysicsJoint::eType_Universal;
	//jointTypes["distance"] = PhysicsJoint::eType_Distance;
	//jointTypes["fixed"] = PhysicsJoint::eType_Fixed;
	//jointTypes["pointInPlane"] = PhysicsJoint::eType_PointInPlane;
	//jointTypes["pointOnLine"] = PhysicsJoint::eType_PointOnLine;
	//jointTypes["prismatic"] = PhysicsJoint::eType_Prismatic;
	//jointTypes["pulley"] = PhysicsJoint::eType_Pulley;
	//jointTypes["revolute"] = PhysicsJoint::eType_Revolute;
	//jointTypes["spherical"] = PhysicsJoint::eType_Spherical;

	////
	//// load physics bodies
	////
	//for (u32 i = 0, iCount = pBodiesElem->getChildren().size(); i < iCount; ++i)
	//{
	//	XmlElement* pBodyElem = pBodiesElem->getChildren()[i];

	//	if (pBodyElem->isValid() && pBodyElem->getName() == "body")
	//	{
	//		PhysicsBodyInfo body;

	//		body.m_bEnabled = pBodyElem->getAttributeValueAsBool("enabled", true);
	//		body.m_bEnableGravity = pBodyElem->getAttributeValueAsBool("enableGravity", true);
	//		body.m_density = pBodyElem->getAttributeValueAsFloat("density", 1);
	//		body.m_mass = pBodyElem->getAttributeValueAsFloat("mass", 0);
	//		body.m_name = pBodyElem->getAttributeValue("name");
	//		body.m_rotation = pBodyElem->getAttributeValueAsQuat("rotation");
	//		body.m_translation = pBodyElem->getAttributeValueAsVector3D("translation");
	//		body.m_behavior = bodyBehaviors[pBodyElem->getAttributeValue("behavior", "static")];

	//		XmlElement* pShapeElem;

	//		// load shapes from body
	//		for (u32 j = 0, jCount = pBodyElem->getChildren().size(); j < jCount; ++j)
	//		{
	//			PhysicsShapeInfo shape;

	//			pShapeElem = pBodyElem->getChildren()[j];

	//			shape.m_density = pShapeElem->getAttributeValueAsFloat("density", 0);
	//			shape.m_mass = pShapeElem->getAttributeValueAsFloat("mass", 1);
	//			shape.m_rotation = pShapeElem->getAttributeValueAsQuat("rotation");
	//			shape.m_translation = pShapeElem->getAttributeValueAsVector3D("translation");
	//			shape.m_type = shapeTypes[pShapeElem->getAttributeValue("type", "box")];

	//			// load specific properties
	//			for (u32 k = 0, kCount = pShapeElem->getChildren().size(); k < kCount; ++k)
	//			{
	//				XmlElement* pPropElem = pShapeElem->getChildren()[k];

	//				shape.m_properties[pPropElem->getAttributeValue("name")] = pPropElem->getAttributeValue("value");
	//			}

	//			body.m_shapes.push_back(shape);
	//		}

	//		m_bodies.push_back(body);
	//	}
	//	else
	//	{
	//		N_LOG_ERROR("Invalid <body> object");
	//	}
	//}

	////
	//// load physics joints
	////
	//for (u32 i = 0, iCount = pJointsElem->getChildren().size(); i < iCount; ++i)
	//{
	//	XmlElement* pJointElem = pJointsElem->getChildren()[i];

	//	if (pJointElem->isValid() && pJointElem->getName() == "joint")
	//	{
	//		PhysicsJointInfo joint;

	//		joint.m_bBreakable = pJointElem->getAttributeValueAsBool("breakable", false);
	//		joint.m_breakForce = pJointElem->getAttributeValueAsFloat("breakForce", 0);
	//		joint.m_breakTorque = pJointElem->getAttributeValueAsFloat("breakTorque", 0);
	//		joint.m_bCollideBodies = pJointElem->getAttributeValueAsBool("collideBodies", false);
	//		joint.m_bMotorized = pJointElem->getAttributeValueAsBool("motorized", false);
	//		joint.m_bodyName[0] = pJointElem->getAttributeValue("body1");
	//		joint.m_bodyName[1] = pJointElem->getAttributeValue("body2");
	//		joint.m_bSpring = pJointElem->getAttributeValueAsBool("useSpring", false);
	//		joint.m_localAxis = pJointElem->getAttributeValueAsVector3D("localAxis");
	//		joint.m_localBodyAnchor[0] = pJointElem->getAttributeValueAsVector3D("localBodyAnchor1");
	//		joint.m_localBodyAnchor[1] = pJointElem->getAttributeValueAsVector3D("localBodyAnchor2");
	//		joint.m_motorForce = pJointElem->getAttributeValueAsFloat("motorForce");
	//		joint.m_motorVelocity = pJointElem->getAttributeValueAsFloat("motorVelocity");
	//		joint.m_spring = pJointElem->getAttributeValueAsFloat("spring");
	//		joint.m_springDamper = pJointElem->getAttributeValueAsFloat("springDamper");
	//		joint.m_springRestingValue = pJointElem->getAttributeValueAsFloat("springRestingValue");
	//		joint.m_translation = pJointElem->getAttributeValueAsVector3D("translation");
	//		joint.m_rotation = pJointElem->getAttributeValueAsQuat("rotation");
	//		joint.m_type = jointTypes[pJointElem->getAttributeValue("type", "undefined")];

	//		m_joints.push_back(joint);
	//	}
	//	else
	//	{
	//		N_LOG_ERROR("Invalid <joint> object");
	//	}
	//}

	return true;
}

bool PhysicsSetupData::unload()
{
	Resource::unload();

	m_bodies.clear();
	m_joints.clear();

	return true;
}

void PhysicsSetupData::createInstances(
							Array<PhysicsBody*>* pArrBodies,
							Array<PhysicsJoint*>* pArrJoints,
							GeometryBuffer* pGbTriangleMesh,
							Array<Vec3>* pConvexPointCloud)
{
	if (pArrBodies)
	{
		for (u32 i = 0, iCount = m_bodies.count(); i < iCount; ++i)
		{
			PhysicsBody* pBody = physics().currentSpace()->createBody();

			N_ASSERT(pBody);
			//pBody->setName(m_bodies[i].m_name.c_str());

			for (u32 j = 0, jCount = m_bodies[i].m_shapes.count(); j < jCount; ++j)
			{
				PhysicsShape*       pShape = nullptr;
				ShapeInfo&   shp = m_bodies[i].m_shapes[j];

				switch (shp.m_type)
				{
				case PhysicsShape::eType_Box:
				{
					f32 width = atof(shp.m_properties["width"].c_str());
					f32 height = atof(shp.m_properties["height"].c_str());
					f32 depth = atof(shp.m_properties["depth"].c_str());
					pShape = physics().currentSpace()->createBoxShape(width, height, depth);
					break;
				}

				case PhysicsShape::eType_Cylinder:
				{
					f32 radius = atof(shp.m_properties["radius"].c_str());
					f32 height = atof(shp.m_properties["height"].c_str());
					u32 segs = atoi(shp.m_properties["segments"].c_str());
					pShape = physics().currentSpace()->createCylinderShape(radius, height, segs);
					break;
				}

				case PhysicsShape::eType_Capsule:
				{
					f32 radius = atof(shp.m_properties["radius"].c_str());
					f32 height = atof(shp.m_properties["height"].c_str());
					pShape = physics().currentSpace()->createCapsuleShape(radius, height);
					break;
				}

				case PhysicsShape::eType_Sphere:
				{
					f32 radius = atof(shp.m_properties["radius"].c_str());
					pShape = physics().currentSpace()->createSphereShape(radius);
					break;
				}

				case PhysicsShape::eType_Plane:
				{
					Vec3 normal = toVec3(shp.m_properties["normal"]);
					f32 dist = atof(shp.m_properties["distance"].c_str());
					pShape = physics().currentSpace()->createPlaneShape(normal, dist);
					break;
				}

				case PhysicsShape::eType_ConvexHull:
				{
					if (pConvexPointCloud)
					{
						pShape = physics().currentSpace()->createConvexHullShape(*pConvexPointCloud);
					}
					break;
				}

				case PhysicsShape::eType_TriangleMesh:
				{
					pShape = physics().currentSpace()->createTriangleMeshShape(pGbTriangleMesh);
					break;
				}

				case PhysicsShape::eType_HeightField:
				{
					// shape not handled in the physics setup file, only used by terrain, but kept for consistency
					break;
				}
				}

				pBody->addShape(pShape);
			}

			pBody->create();
			pArrBodies->append(pBody);
		}
	}

	if (pArrJoints)
	{
		for (u32 i = 0, iCount = m_joints.count(); i < iCount; ++i)
		{
			JointInfo& jnt = m_joints[i];
			PhysicsJoint* pJoint = physics().currentSpace()->createJoint(jnt.m_type);

			N_ASSERT(pJoint);

			if (pArrBodies)
			{
				//pJoint->linkBodies(
				//					findBodyByName(
				//						*pArrBodies,
				//						jnt.m_bodyName[0].c_str()),
				//					findBodyByName(
				//						*pArrBodies,
				//						jnt.m_bodyName[1].c_str()));
			}

			pJoint->setBreakable(jnt.m_bBreakable, jnt.m_breakForce, jnt.m_breakTorque);
			pJoint->setCollisionBetweenBodies(jnt.m_bCollideBodies);
			pJoint->setMotor(jnt.m_bMotorized, jnt.m_motorForce, jnt.m_motorVelocity);
			pJoint->setSpring(jnt.m_bSpring, jnt.m_spring, jnt.m_springDamper, jnt.m_springRestingValue);
			pJoint->setLocalAxis(jnt.m_localAxis);
			pJoint->setLocalBodyAnchor(0, jnt.m_localBodyAnchor[0]);
			pJoint->setLocalBodyAnchor(1, jnt.m_localBodyAnchor[1]);
			//TODO: dangerous, no notification of setting it, add protected in Transform and add accessors
			pJoint->setTranslation(jnt.m_translation);
			pJoint->setRotation(jnt.m_rotation);
			pJoint->create();

			pArrJoints->append(pJoint);
		}
	}
}

const Array<PhysicsSetupData::BodyInfo>& PhysicsSetupData::bodies() const
{
	return m_bodies;
}

const Array<PhysicsSetupData::JointInfo>& PhysicsSetupData::joints() const
{
	return m_joints;
}

PhysicsBody* PhysicsSetupData::findBodyByName(const Array<PhysicsBody*>& rBodies, const char* pName)
{
	//for (u32 i = 0, iCount = rBodies.count(); i < iCount; ++i)
	//{
	//	if (rBodies[i]->name() == pName)
	//	{
	//		return rBodies[i];
	//	}
	//}

	return nullptr;
}

}