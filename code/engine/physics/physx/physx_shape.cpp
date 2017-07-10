#include "common.h"
#include "physics/physx/physx_manager.h"
#include "physics/space.h"
#include "physics/physx/physx_body.h"
#include "physx_shape.h"
#include "base/math/util.h"
#include "core/core.h"
#include "render/geometry_buffer.h"
#include "render/vertex_buffer.h"
#include "render/index_buffer.h"

namespace engine
{
PhysXShape::PhysXShape()
{
	m_pNxShapeDesc = nullptr;
	m_pNxShape = nullptr;
}

PhysXShape::~PhysXShape()
{
}

void PhysXShape::free()
{
	if (m_pParentBody && m_pNxShape)
	{
		if (((PhysXBody*)m_pParentBody)->m_pNxActor)
		{
			//((PhysXBody*)m_pParentBody)->m_pNxActor->releaseShape( *m_pNxShape );
		}
	}

	m_pNxShape = nullptr;
}

void PhysXShape::setTranslation(f32 aX, f32 aY, f32 aZ)
{
	m_translation = Vec3(aX, aY, aZ);

	if (m_pNxShape)
	{
		m_pNxShape->setLocalPosition(NxVec3(aX, aY, aZ));

		if (m_pParentBody)
		{
			if (((PhysXBody*)m_pParentBody)->m_pNxActor)
			{
				m_pParentBody->setMass(m_pParentBody->mass());
			}
		}
	}
}

void PhysXShape::setMaterial(PhysicsMaterial* pMaterial)
{
	PhysicsShape::setMaterial(pMaterial);

	if (m_pNxShape)
	{
		PhysXMaterial* pMat = (PhysXMaterial*)pMaterial;
		m_pNxShape->setMaterial(pMat->m_pNxMaterial->getMaterialIndex());
	}
}

//---

PhysXBoxShape::PhysXBoxShape()
{
	m_pNxShape = nullptr;
	m_pNxShapeDesc = &m_nxShapeDesc;
}

PhysXBoxShape::~PhysXBoxShape()
{
}

bool PhysXBoxShape::create(f32 aWidth, f32 aHeight, f32 aDepth)
{
	m_nxShapeDesc.setToDefault();
	m_nxShapeDesc.dimensions.set(aWidth / 2.0f, aHeight / 2.0f, aDepth / 2.0f);

	return true;
}

f32 PhysXBoxShape::boxWidth() const
{
	return m_nxShapeDesc.dimensions.x * 2.0f;
}

f32 PhysXBoxShape::boxHeight() const
{
	return m_nxShapeDesc.dimensions.y * 2.0f;
}

f32 PhysXBoxShape::boxDepth() const
{
	return m_nxShapeDesc.dimensions.z * 2.0f;
}

void PhysXBoxShape::getBox(BBox& rOutBox) const
{
	rOutBox.m_min = Vec3(
					-m_nxShapeDesc.dimensions.x,
					-m_nxShapeDesc.dimensions.y,
					-m_nxShapeDesc.dimensions.z);
	rOutBox.m_max = Vec3(
					m_nxShapeDesc.dimensions.x,
					m_nxShapeDesc.dimensions.y,
					m_nxShapeDesc.dimensions.z);
}

//---

PhysXCylinderShape::PhysXCylinderShape()
{
	m_pNxShape = nullptr;
	m_pNxShapeDesc = &m_nxShapeDesc;
}

PhysXCylinderShape::~PhysXCylinderShape()
{
}

bool PhysXCylinderShape::create(
								f32 aRadius,
								f32 aHeight,
								u32 aSideCount /* = 30 */)
{
	m_cylinderRadius = aRadius;
	m_cylinderHeight = aHeight;
	m_cylinderSideCount = aSideCount;

	// create cylinder points
	f32 midHeight = m_cylinderHeight / 2.0f;
	f32 angle = 0, angleStep = (f32)kTwoPi / m_cylinderSideCount;
	f32 cylX, cylZ;
	Vec3 pt;
	Array<Vec3> pointCloud;

	// cylinder caps
	for (u32 i = 0; i < m_cylinderSideCount; ++i)
	{
		angle += angleStep;
		cylX = m_cylinderRadius * sin(angle);
		cylZ = m_cylinderRadius * cos(angle);
		// top
		pt.set(cylX, midHeight, cylZ);
		pointCloud.append(pt);
		// bottom
		pt.set(cylX, -midHeight, cylZ);
		pointCloud.append(pt);
	}

	NxConvexMeshDesc meshDesc;

	meshDesc.numVertices = pointCloud.size();
	meshDesc.pointStrideBytes = sizeof(Vec3);
	meshDesc.points = &pointCloud[0].x;
	meshDesc.flags = NX_CF_COMPUTE_CONVEX ;

	NxCookingInterface* pCook = ((PhysXManager*)&physics())->m_pNxCooking;
	MemoryWriteBuffer buf;

	if (!pCook)
		return false;

	m_nxShapeDesc.setToDefault();

	if (pCook->NxCookConvexMesh(meshDesc, buf))
	{
		s_simulationMutex.lock();
		m_nxShapeDesc.meshData =
			((PhysXManager*)&physics())->m_pNxPhysicsSDK->createConvexMesh(
											MemoryReadBuffer(buf.data));
		s_simulationMutex.unlock();
		B_ASSERT(m_nxShapeDesc.meshData);
	}

	return true;
}

f32 PhysXCylinderShape::cylinderRadius() const
{
	return m_cylinderRadius;
}

f32 PhysXCylinderShape::cylinderHeight() const
{
	return m_cylinderHeight;
}

u32 PhysXCylinderShape::cylinderSideCount() const
{
	return m_cylinderSideCount;
}

//---

PhysXCapsuleShape::PhysXCapsuleShape()
{
	m_pNxShape = nullptr;
	m_pNxShapeDesc = &m_nxShapeDesc;
}

PhysXCapsuleShape::~PhysXCapsuleShape()
{
}

bool PhysXCapsuleShape::create(f32 aRadius, f32 aHeight)
{
	m_nxShapeDesc.setToDefault();
	m_nxShapeDesc.radius = aRadius;
	m_nxShapeDesc.height = aHeight;

	return true;
}

f32 PhysXCapsuleShape::capsuleRadius() const
{
	return m_nxShapeDesc.radius;
}

f32 PhysXCapsuleShape::capsuleHeight() const
{
	return m_nxShapeDesc.height;
}

//---

PhysXSphereShape::PhysXSphereShape()
{
	m_pNxShape = nullptr;
	m_pNxShapeDesc = &m_nxShapeDesc;
}

PhysXSphereShape::~PhysXSphereShape()
{
}

bool PhysXSphereShape::create(f32 aRadius)
{
	m_nxShapeDesc.setToDefault();
	m_nxShapeDesc.radius = aRadius;

	return true;
}

f32 PhysXSphereShape::sphereRadius() const
{
	return m_nxShapeDesc.radius;
}

//---

PhysXPlaneShape::PhysXPlaneShape()
{
	m_pNxShape = nullptr;
	m_pNxShapeDesc = &m_nxShapeDesc;
}

PhysXPlaneShape::~PhysXPlaneShape()
{
}

bool PhysXPlaneShape::create(const Vec3& rNormal, f32 aDistanceAlongNormal /* = 0 */)
{
	m_nxShapeDesc.setToDefault();
	m_nxShapeDesc.normal.set(rNormal.x, rNormal.y, rNormal.z);
	m_nxShapeDesc.d = aDistanceAlongNormal;
	m_nxShapeDesc.shapeFlags |= NX_SF_VISUALIZATION;

	return true;
}

Vec3 PhysXPlaneShape::planeNormal() const
{
	return Vec3(m_nxShapeDesc.normal.x, m_nxShapeDesc.normal.y, m_nxShapeDesc.normal.z);
}

f32 PhysXPlaneShape::planeDistanceAlongNormal() const
{
	return m_nxShapeDesc.d;
}

//---

PhysXConvexHullShape::PhysXConvexHullShape()
{
	m_pNxShape = nullptr;
	m_pNxShapeDesc = &m_nxShapeDesc;
}

PhysXConvexHullShape::~PhysXConvexHullShape()
{
}

bool PhysXConvexHullShape::create(const Array<Vec3>& rPointCloud)
{
	NxConvexMeshDesc convexDesc;

	convexDesc.numVertices = rPointCloud.size();
	convexDesc.pointStrideBytes = sizeof(Vec3);
	convexDesc.points = &rPointCloud[0].x;
	convexDesc.flags = NX_CF_COMPUTE_CONVEX ;

	NxCookingInterface* pCook = ((PhysXManager*)&physics())->m_pNxCooking;
	MemoryWriteBuffer buf;

	if (!pCook)
		return false;

	m_nxShapeDesc.setToDefault();

	if (pCook->NxCookConvexMesh(convexDesc, buf))
	{
		s_simulationMutex.lock();
		m_nxShapeDesc.meshData =
			((PhysXManager*)&physics())
						->m_pNxPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
		s_simulationMutex.unlock();
		B_ASSERT(m_nxShapeDesc.meshData);
	}

	return true;
}

//---

PhysXTriangleMeshShape::PhysXTriangleMeshShape()
{
	m_pNxShape = nullptr;
	m_pNxShapeDesc = &m_nxShapeDesc;
}

PhysXTriangleMeshShape::~PhysXTriangleMeshShape()
{
}

bool PhysXTriangleMeshShape::create(GeometryBuffer* pGbTriMesh)
{
	NxTriangleMeshDesc meshDesc;

	B_ASSERT(pGbTriMesh);

	if (!pGbTriMesh)
		return false;

	meshDesc.numVertices = pGbTriMesh->vertexBuffer()->elementCount();
	meshDesc.numTriangles = pGbTriMesh->indexBuffer()->elementCount() / 3;
	meshDesc.pointStrideBytes = sizeof(Vec3);
	meshDesc.triangleStrideBytes = 3 * sizeof(u32);
	meshDesc.points = pGbTriMesh->vertexBuffer()->mapFull();
	meshDesc.triangles = pGbTriMesh->indexBuffer()->mapFull();
	meshDesc.flags = 0;

	NxCookingInterface* pCook = ((PhysXManager*)&physics())->m_pNxCooking;
	MemoryWriteBuffer buf;

	if (!pCook)
	{
		pGbTriMesh->vertexBuffer()->unmap();
		pGbTriMesh->indexBuffer()->unmap();
		return false;
	}

	m_nxShapeDesc.setToDefault();

	if (pCook->NxCookTriangleMesh(meshDesc, buf))
	{
		s_simulationMutex.lock();
		m_nxShapeDesc.meshData = ((PhysXManager*)&physics())
									->m_pNxPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
		s_simulationMutex.unlock();
		B_ASSERT(m_nxShapeDesc.meshData);
	}
	
	pGbTriMesh->vertexBuffer()->unmap();
	pGbTriMesh->indexBuffer()->unmap();

	return true;
}

//---

PhysXHeightFieldShape::PhysXHeightFieldShape()
{
	m_pNxShape = nullptr;
	m_pNxShapeDesc = &m_nxShapeDesc;
}

PhysXHeightFieldShape::~PhysXHeightFieldShape()
{
}

bool PhysXHeightFieldShape::create(const Table<f32>& rHeightField)
{
	NxReal sixtyFourKb = 65536.0f;
	NxReal thirtyTwoKb = 32767.5f;
	NxHeightFieldDesc heightFieldDesc;

	heightFieldDesc.nbColumns = rHeightField.width();
	heightFieldDesc.nbRows = rHeightField.height();
	heightFieldDesc.verticalExtent = -1000;
	heightFieldDesc.convexEdgeThreshold = 0;

	heightFieldDesc.samples = new NxU32[rHeightField.length()];
	B_ASSERT(heightFieldDesc.samples);
	heightFieldDesc.sampleStride = sizeof(NxU32);

	u8* pCurrentByte = (u8*)heightFieldDesc.samples;

	for (u32 row = 0, rowCount = rHeightField.width(); row < rowCount; ++row)
	{
		for (u32 column = 0, colCount = rHeightField.height(); column < colCount; ++column)
		{
			NxI16 height = (NxI32)(thirtyTwoKb * rHeightField.value(row, column));
			NxHeightFieldSample* currentSample = (NxHeightFieldSample*)pCurrentByte;

			currentSample->height = height;
			currentSample->materialIndex0 = 0;
			currentSample->materialIndex1 = 0;
			currentSample->tessFlag = 0;
			pCurrentByte += heightFieldDesc.sampleStride;
		}
	}

	s_simulationMutex.lock();
	NxHeightField* pHeightField =
		((PhysXManager*)&physics())->m_pNxPhysicsSDK->createHeightField(
														heightFieldDesc);
	s_simulationMutex.unlock();
	B_ASSERT(pHeightField);

	// Data has been copied, we can free our buffer
	delete [](NxU32*)(heightFieldDesc.samples);

	m_nxShapeDesc.setToDefault();

	if (pHeightField)
	{
		m_nxShapeDesc.heightField = pHeightField;
		m_nxShapeDesc.shapeFlags = NX_SF_FEATURE_INDICES | NX_SF_VISUALIZATION;
		m_nxShapeDesc.heightScale = m_scale.y / thirtyTwoKb;
		m_nxShapeDesc.rowScale = m_scale.x / NxReal(rHeightField.width() - 1);
		m_nxShapeDesc.columnScale = m_scale.z / NxReal(rHeightField.height() - 1);
		B_LOG_INFO("Heightfield info: scale %f %f %f, heightmap: %dx%d",
			m_scale.x, m_scale.y, m_scale.z, rHeightField.width(), rHeightField.height());
		//heightFieldShapeDesc.meshFlags  = NX_MESH_SMOOTH_SPHERE_COLLISIONS;
		m_nxShapeDesc.materialIndexHighBits = 0;
		m_nxShapeDesc.holeMaterial = 1;
	}

	return true;
}
}