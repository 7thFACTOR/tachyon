#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/resource.h"
#include "base/math/matrix.h"

namespace engine
{
class GeometryBuffer;
struct MeshPart;
class SkeletonJoint;

enum EMeshChunkIds
{
	eMeshChunk_Header,
	eMeshChunk_Bones,
	eMeshChunk_GeometryBuffers,
	eMeshChunk_Parts,
	eMeshChunk_Hulls
};

//! The mesh data hold only the actual naked mesh data (no textures or gpu program info)
class E_API Mesh : public Resource
{
public:
	static const int kMeshFileVersion = 1;
	B_RUNTIME_CLASS;
	Mesh();
	virtual ~Mesh();

	//! \return the mesh's available geometry buffers array
	const Array<GeometryBuffer*>& geometryBuffers() const { return m_geometryBuffers; }
	const Array<MeshPart*>& meshParts() const { return m_parts; }
	MeshPart* findMeshPartByName(const char* pName) const;
	//! load the mesh from a .mesh file
	bool load(Stream* pStream);
	//! unload the mesh data
	bool unload();
	//! \return bones
	const Array<SkeletonJoint*>& bones() const;
	//! \return the bind shape matrix, used in skinning
	const Matrix& bindShapeMatrix() const;
	//! \return the simplified hull mesh used in collisions, occlusion tests, etc.
	const Array<GeometryBuffer*>& hulls() const;
	//! set bind matrix
	void setBindShapeMatrix(const Matrix& rValue);

protected:
	Array<GeometryBuffer*> m_geometryBuffers;
	Array<MeshPart*> m_parts;
	//! contains only the names of the bones, the map's key is the bone name
	Array<SkeletonJoint*> m_bones;
	//! the bind pose shape/mesh matrix, used in skinning
	Matrix m_bindShapeMatrix;
	//! the hull mesh, simplified, low detail mesh, used for physics, ray intersection tests, occlusion
	Array<GeometryBuffer*> m_hulls;
	//! mesh flags
	u32 m_meshFlags;
};
}