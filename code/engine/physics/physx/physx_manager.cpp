#include "common.h"
#include "physics/physx/physx_manager.h"
#include "physics/physx/physx_joint.h"
#include "physics/physx/physx_body.h"
#include "physx_character.h"
#include "physx_shape.h"
#include "game/components/camera.h"
#include "render/render_manager.h"
#include "core/core.h"

namespace engine
{
#define USE_PHYSX_VISUAL_REMOTE_DEBUGGER
#define VRD_HOST "localhost"
#define VRD_PORT NX_DBG_DEFAULT_PORT
#define VRD_EVENTMASK NX_DBG_EVENTMASK_EVERYTHING

B_DEFINE_RUNTIME_CLASS(PhysXManager);

//---

static class PhysXUserOutputStream : public NxUserOutputStream
{
public:
	void reportError(NxErrorCode code, const char * message, const char *file, i32 line)
	{
		B_LOG_ERROR("PhysX Error(%d):'%s' in %s at %d", code, message, file, line);
	}

	NxAssertResponse reportAssertViolation(const char * message, const char *file, i32 line)
	{
		B_LOG_ERROR("PhysX Assert:'%s' in %s at %d", message, file, line);
		return NX_AR_CONTINUE;
	}

	void print(const char * message)
	{
		B_LOG_WARNING("PhysX:'%s'", message);
	}
} s_PhysXUserOutputStream;

//---

class MyNxUserAllocator : public NxUserAllocator
{
	void* mallocDEBUG(size_t size, const char* fileName, int line)
	{
		return ::malloc(size);
	}

	void* malloc(size_t size)
	{
		return ::malloc(size);
	}

	void* realloc(void* memory, size_t size)
	{
		return ::realloc(memory, size);
	}

	void free(void* memory)
	{
		::free(memory);
	}
} s_PhysXAllocator;

//---

PhysXMaterial::PhysXMaterial() : PhysicsMaterial()
{
	m_pNxMaterial = nullptr;
}

PhysXMaterial::~PhysXMaterial()
{
	if (!m_pNxMaterial)
		return;

	m_pNxMaterial->getScene().releaseMaterial(*m_pNxMaterial);
}

void PhysXMaterial::update()
{
	if (!m_pNxMaterial)
		return;

	m_pNxMaterial->setDynamicFriction(m_dynamicFriction);
	m_pNxMaterial->setStaticFriction(m_staticFriction);
	m_pNxMaterial->setRestitution(m_restitution);
}

//---

void PhysXUserContactReport::onContactNotify(NxContactPair& pair, NxU32 events)
{
	if (m_pSpace->listener())
	{
		PhysXBody* pBody1 = m_pSpace->getPhysXBodyByNxActor(pair.actors[0]);
		PhysXBody* pBody2 = m_pSpace->getPhysXBodyByNxActor(pair.actors[1]);

		m_contacts.clear();

		// Iterate through contact points
		NxContactStreamIterator it(pair.stream);

		while (it.goNextPair())
		{
			//user can also call getShape() and getNumPatches() here
			while (it.goNextPatch())
			{
				PhysicsContactPairInfo pairInfo;

				//user can also call getPatchNormal() and getNumPoints() here
				const NxVec3& contactNormal = it.getPatchNormal();

				pairInfo.normal = Vec3(contactNormal.x, contactNormal.y, contactNormal.z);

				while (it.goNextPoint())
				{
					//user can also call getPoint() and getSeparation() here
					const NxVec3& contactPoint = it.getPoint();

					if (it.getPointNormalForce() > 15.8)
					{
						pairInfo.points.append(Vec3(contactPoint.x, contactPoint.y, contactPoint.z));
					}
				}

				m_contacts.append(pairInfo);
			}
		}

		m_pSpace->listener()->onContact(pBody1, pBody2, m_contacts);
	}
}

//---

PhysXSpace::PhysXSpace() : PhysicsSpace()
{
	m_pNxScene = nullptr;
	m_pNxProvider = nullptr;
	m_onContactReport.m_pSpace = this;
}

PhysXSpace::~PhysXSpace()
{
	free();
}

bool PhysXSpace::create()
{
	NxSceneDesc sceneDesc;

	free();
	
	if (!m_pNxProvider->m_pNxPhysicsSDK)
	{
		return false;
	}

	sceneDesc.gravity = NxVec3(m_gravity.x, m_gravity.y, m_gravity.z);
	s_simulationMutex.lock();
	m_pNxScene = m_pNxProvider->m_pNxPhysicsSDK->createScene(sceneDesc);
	s_simulationMutex.unlock();
	B_ASSERT(m_pNxScene);

	if (!m_pNxScene)
		return false;

	// set default material
	NxMaterial* defaultMaterial = m_pNxScene->getMaterialFromIndex(0);
	B_ASSERT(defaultMaterial);
	defaultMaterial->setRestitution(0.1f);
	defaultMaterial->setStaticFriction(0.2f);
	defaultMaterial->setDynamicFriction(0.5f);
	m_pNxScene->setUserContactReport((NxUserContactReport*)&m_onContactReport);
	m_pNxScene->setActorGroupPairFlags(ePhysXGroup_Collidable, ePhysXGroup_Collidable, NX_NOTIFY_ON_TOUCH);

	return true;
}

void PhysXSpace::free()
{
	if (m_pNxScene)
	{
		m_pNxScene->shutdownWorkerThreads();
		m_pNxProvider->m_pNxPhysicsSDK->releaseScene(*m_pNxScene);
		m_pNxScene = nullptr;
	}

	m_bodies.clear();
}

void PhysXSpace::setListener(PhysicsEventListener* pHandler)
{
	PhysicsSpace::setListener(pHandler);
}

PhysicsBody* PhysXSpace::createBody()
{
	PhysXBody* pBody = new PhysXBody();

	B_ASSERT(pBody);
	pBody->setPhysicsSpace(this);
	m_bodies.append(pBody);

	return pBody;
}

PhysicsCharacter* PhysXSpace::createCharacter()
{
	PhysXCharacter* pChr = new PhysXCharacter();

	pChr->m_pNxScene = m_pNxScene;

	return pChr;
}

PhysicsMaterial* PhysXSpace::createMaterial()
{
	PhysXMaterial* pMat = new PhysXMaterial();

	B_ASSERT(pMat);
	NxMaterialDesc mat;
	mat.setToDefault();
	s_simulationMutex.lock();
	pMat->m_pNxMaterial = m_pNxScene->createMaterial(mat);
	s_simulationMutex.unlock();
	B_ASSERT(pMat->m_pNxMaterial);

	return pMat;
}

PhysicsJoint* PhysXSpace::createJoint(PhysicsJoint::EType aType)
{
	switch (aType)
	{
	case PhysicsJoint::eType_Cylindrical:
		{
			PhysXCylindricalJoint* pJoint = new PhysXCylindricalJoint;

			B_ASSERT(pJoint);
			pJoint->m_pNxScene = m_pNxScene;
			pJoint->setPhysicsSpace(this);
			return pJoint;
		}

	case PhysicsJoint::eType_Universal:
		{
			PhysXUniversalJoint* pJoint = new PhysXUniversalJoint;

			B_ASSERT(pJoint);
			pJoint->m_pNxScene = m_pNxScene;
			pJoint->setPhysicsSpace(this);
			return pJoint;
		}

	case PhysicsJoint::eType_Distance:
		{
			PhysXDistanceJoint* pJoint = new PhysXDistanceJoint;

			B_ASSERT(pJoint);
			pJoint->m_pNxScene = m_pNxScene;
			pJoint->setPhysicsSpace(this);
			return pJoint;
		}

	case PhysicsJoint::eType_Fixed:
		{
			PhysXFixedJoint* pJoint = new PhysXFixedJoint;

			B_ASSERT(pJoint);
			pJoint->m_pNxScene = m_pNxScene;
			pJoint->setPhysicsSpace(this);
			return pJoint;
		}

	case PhysicsJoint::eType_PointInPlane:
		{
			PhysXPointInPlaneJoint* pJoint = new PhysXPointInPlaneJoint;

			B_ASSERT(pJoint);
			pJoint->m_pNxScene = m_pNxScene;
			pJoint->setPhysicsSpace(this);
			return pJoint;
		}

	case PhysicsJoint::eType_PointOnLine:
		{
			PhysXPointOnLineJoint* pJoint = new PhysXPointOnLineJoint;

			B_ASSERT(pJoint);
			pJoint->m_pNxScene = m_pNxScene;
			pJoint->setPhysicsSpace(this);
			return pJoint;
		}

	case PhysicsJoint::eType_Prismatic:
		{
			PhysXPrismaticJoint* pJoint = new PhysXPrismaticJoint;

			B_ASSERT(pJoint);
			pJoint->m_pNxScene = m_pNxScene;
			pJoint->setPhysicsSpace(this);
			return pJoint;
		}

	case PhysicsJoint::eType_Pulley:
		{
			PhysXPulleyJoint* pJoint = new PhysXPulleyJoint;

			B_ASSERT(pJoint);
			pJoint->m_pNxScene = m_pNxScene;
			pJoint->setPhysicsSpace(this);
			return pJoint;
		}

	case PhysicsJoint::eType_Revolute:
		{
			PhysXRevoluteJoint* pJoint = new PhysXRevoluteJoint;

			B_ASSERT(pJoint);
			pJoint->m_pNxScene = m_pNxScene;
			pJoint->setPhysicsSpace(this);
			return pJoint;
		}

	case PhysicsJoint::eType_Spherical:
		{
			PhysXSphericalJoint* pJoint = new PhysXSphericalJoint;

			B_ASSERT(pJoint);
			pJoint->m_pNxScene = m_pNxScene;
			pJoint->setPhysicsSpace(this);
			return pJoint;
		}
	}

	return nullptr;
}

PhysicsForceField* PhysXSpace::createForceField(PhysicsForceField::EType aType)
{
	return nullptr;
}

PhysicsCloth* PhysXSpace::createCloth()
{
	return nullptr;
}

PhysicsRagDoll* PhysXSpace::createRagDoll()
{
	return nullptr;
}

PhysicsRope* PhysXSpace::createRope()
{
	return nullptr;
}

PhysicsSoftBody* PhysXSpace::createSoftBody()
{
	return nullptr;
}

PhysicsVehicle* PhysXSpace::createVehicle()
{
	return nullptr;
}

//---

void PhysXSpace::setGravity(const Vec3& rGravity)
{
	m_gravity = rGravity;
	NxVec3 vec(rGravity.x, rGravity.y, rGravity.z);
	m_pNxScene->setGravity(vec);
}

void PhysXSpace::setGravity(f32 aX, f32 aY, f32 aZ)
{
	m_gravity.set(aX, aY, aZ);
	NxVec3 vec(aX, aY, aZ);
	m_pNxScene->setGravity(vec);
}

PhysicsShape* PhysXSpace::createBoxShape(f32 aWidth, f32 aHeight, f32 aDepth)
{
	PhysXBoxShape* pShape = new PhysXBoxShape();
	pShape->create(aWidth, aHeight, aDepth);
	return pShape;
}

PhysicsShape* PhysXSpace::createCylinderShape(f32 aRadius, f32 aHeight, size_t aSegments)
{
	PhysXCylinderShape* pShape = new PhysXCylinderShape();
	pShape->create(aRadius, aHeight, aSegments);
	return pShape;
}

PhysicsShape* PhysXSpace::createCapsuleShape(f32 aRadius, f32 aHeight)
{
	PhysXCapsuleShape* pShape = new PhysXCapsuleShape();
	pShape->create(aRadius, aHeight);
	return pShape;
}

PhysicsShape* PhysXSpace::createSphereShape(f32 aRadius)
{
	PhysXSphereShape* pShape = new PhysXSphereShape();
	pShape->create(aRadius);
	return pShape;
}

PhysicsShape* PhysXSpace::createPlaneShape(const Vec3& rPlaneNormal, f32 aDistanceAlongNormal)
{
	PhysXPlaneShape* pShape = new PhysXPlaneShape();
	pShape->create(rPlaneNormal, aDistanceAlongNormal);
	return pShape;
}

PhysicsShape* PhysXSpace::createConvexHullShape(const Array<Vec3>& rPointCloud)
{
	PhysXConvexHullShape* pShape = new PhysXConvexHullShape();
	pShape->create(rPointCloud);
	return pShape;
}

PhysicsShape* PhysXSpace::createTriangleMeshShape(GeometryBuffer* pMesh)
{
	PhysXTriangleMeshShape* pShape = new PhysXTriangleMeshShape();
	pShape->create(pMesh);
	return pShape;
}

PhysicsShape* PhysXSpace::createHeightFieldShape(const Table<f32>& rHeightMap)
{
	PhysXHeightFieldShape* pShape = new PhysXHeightFieldShape();
	pShape->create(rHeightMap);
	return pShape;
}

PhysXBody* PhysXSpace::getPhysXBodyByNxActor(NxActor* pActor)
{
	for (u32 i = 0, iCount = m_bodies.size(); i < iCount; ++i)
	{
		PhysXBody* pBody = (PhysXBody*)m_bodies[i];

		if (pBody->m_pNxActor == pActor)
		{
			return pBody;
		}
	}

	return nullptr;
}

//---

PhysXManager::PhysXManager()
{
	m_pNxPhysicsSDK = nullptr;
	m_pNxCtrlManager = nullptr;
	m_pNxCooking = nullptr;
}

PhysXManager::~PhysXManager()
{
	if (m_pNxCtrlManager)
	{
		NxReleaseControllerManager(m_pNxCtrlManager);
	}

	if (m_pNxCooking)
	{
		m_pNxCooking->NxCloseCooking();
	}

	if (m_pNxPhysicsSDK)
	{
#ifdef USE_PHYSX_VISUAL_REMOTE_DEBUGGER
		if (m_pNxPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
		{
			m_pNxPhysicsSDK->getFoundationSDK().getRemoteDebugger()->disconnect();
		}
#endif
		NxReleasePhysicsSDK(m_pNxPhysicsSDK);
	}
}

PhysicsSpace* PhysXManager::newPhysicsSpace()
{
	PhysXSpace* pSpace = new PhysXSpace;

	B_ASSERT(pSpace);
	pSpace->m_pNxProvider = this;
	m_pCurrentPhysicsSpace = pSpace;
	return pSpace;
}

bool PhysXManager::initialize()
{
	static NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;

	m_pNxPhysicsSDK = NxCreatePhysicsSDK(
							NX_PHYSICS_SDK_VERSION,
							nullptr,
							&s_PhysXUserOutputStream,
							desc,
							&errorCode);

	if (!m_pNxPhysicsSDK)
	{
		B_LOG_ERROR("  PhysX SDK could not be started! (code: %d).", errorCode);
		return false;
	}

	m_pNxPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.05f);
	B_LOG_SUCCESS("  PhysX SDK started (code: %d).", errorCode);
	m_pNxCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);

#ifdef USE_PHYSX_VISUAL_REMOTE_DEBUGGER
	m_pNxPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect( VRD_HOST, VRD_PORT, VRD_EVENTMASK );
#endif

	if (m_pNxCooking)
	{
		if (m_pNxCooking->NxInitCooking())
		{
			B_LOG_SUCCESS("  PhysX Mesh Cooking library started OK.");
		}
		else
		{
			B_LOG_ERROR("  PhysX Mesh Cooking library could not be started");
		}
	}
	else
	{
		B_LOG_WARNING("  PhysX Mesh Cooking (DLL) library not found!");
	}

	if (!m_pNxCtrlManager)
	{
		m_pNxCtrlManager = NxCreateControllerManager(&s_PhysXAllocator);
		B_ASSERT(m_pNxCtrlManager);
	}

	m_pNxPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 0);
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_WORLD_AXES, 1);
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_BODY_AXES , 1);
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_BODY_LIN_VELOCITY, 1);
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AABBS, 1);
	m_pNxPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);

	return true;
}

Mutex s_simulationMutex;
bool s_bVisualDebug = true;

void PhysXManager::simulate(f32 aStep)
{
	PhysXSpace* pScene = (PhysXSpace*)m_pCurrentPhysicsSpace;

	s_simulationMutex.lock();
	pScene->m_pNxScene->simulate(aStep);
	pScene->m_pNxScene->flushStream();
	pScene->m_pNxScene->fetchResults(NX_ALL_FINISHED, true);
	s_simulationMutex.unlock();

	m_pNxCtrlManager->updateControllers();

	if (s_bVisualDebug)
	{
		//render().beginRender();
		//render().setWorldMatrix(Matrix());
		//render().depthStencilState.bDepthTest = false;
		//render().commitDepthStencilState();
		//render().disableTexturing();

		//const NxDebugRenderable& data = *(pScene->m_pNxScene->getDebugRenderable());

		////
		//// Render lines
		////
		//{
		//	NxU32 NbLines = data.getNbLines();
		//	const NxDebugLine* Lines = data.getLines();
		//	render().beginPrimitives(eGfxPrimitive_LineList);

		//	while (NbLines--)
		//	{
		//		NxF32 Blue = NxF32((Lines->color) & 0xff) / 255.0f;
		//		NxF32 Green = NxF32((Lines->color >> 8) & 0xff) / 255.0f;
		//		NxF32 Red = NxF32((Lines->color >> 16) & 0xff) / 255.0f;

		//		render().setColor(Red, Green, Blue, 1);
		//		render().addVertex(Lines->p0.x, Lines->p0.y, Lines->p0.z);
		//		render().addVertex(Lines->p1.x, Lines->p1.y, Lines->p1.z);
		//		++Lines;
		//	}

		//	render().endPrimitives();
		//}

		////
		//// Render tris
		////
		//{
		//	NxU32 NbTris = data.getNbTriangles();
		//	const NxDebugTriangle* Tris = data.getTriangles();
		//	render().beginPrimitives(eGfxPrimitive_TriangleList);

		//	while (NbTris--)
		//	{
		//		NxF32 Blue = NxF32((Tris->color) & 0xff) / 255.0f;
		//		NxF32 Green = NxF32((Tris->color >> 8) & 0xff) / 255.0f;
		//		NxF32 Red = NxF32((Tris->color >> 16) & 0xff) / 255.0f;

		//		render().setColor(Red, Green, Blue, 1);
		//		render().addVertex(Tris->p0.x, Tris->p0.y, Tris->p0.z);
		//		render().addVertex(Tris->p1.x, Tris->p1.y, Tris->p1.z);
		//		render().addVertex(Tris->p2.x, Tris->p2.y, Tris->p2.z);
		//		++Tris;
		//	}

		//	render().endPrimitives();
		//}

		//render().endRender();
	}
}
}