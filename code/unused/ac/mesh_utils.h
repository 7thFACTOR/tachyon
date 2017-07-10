#pragma once
#include "common.h"
#include "asset_processor.h"
#include "base/math/color.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/matrix.h"
#include "base/math/bbox.h"
#include "base/array.h"
#include "base/string.h"
#include "base/map.h"

namespace ac
{
using namespace base;
const int kMaxBonesPerVertex = 4;
const int kMaxUvSets = 4;

class Animation;
class MeshPart;

namespace MeshChunkIds
{
	enum MeshChunkId
	{
		Header,
		Joints,
		GeometryBuffers,
		Hull
	};
}
typedef MeshChunkIds::MeshChunkId MeshChunkId;

enum ESkeletonAnimChunkIds
{
	eSkeletonAnimChunk_Header = 1,
	eSkeletonAnimChunk_Bones
};

struct TexCoord
{
	f32 u, v;
};

struct Vertex
{
	Vec3 position, normal, tangent, bitangent;
	Color color;
	TexCoord uv[kMaxUvSets];
};

class CommonInfo
{
public:
	String m_name, m_parentName, m_dccTool, m_author, m_date, m_comments;
};

class NodeTransform
{
public:
	NodeTransform()
		: m_scale(1, 1, 1)
	{
	}

	Vec3 m_translation;
	Quat m_rotation;
	Vec3 m_scale;
	BBox m_bbox;
	Matrix m_bindPoseInverse, m_localTransform;
};

class Node : public CommonInfo, public NodeTransform
{
public:
	Node();
	Node(const Node& rOther);
	virtual ~Node();

	Node& operator = (const Node& rOther);

	Node* m_pParent;
	Array<Node*> m_children;
};

class Material
{
public:
	Material()
	{
		m_gpuProgram = "default";
	}

	String m_name, m_gpuProgram, m_physicsMaterial;
	Map<String, String> m_textureFiles;
	void* m_pHandle;
};

class VertexWeights
{
public:
	// map of bone index and weight
	Map<i32, f32> m_boneWeights;
};

class SubMeshVertexWeights
{
public:
	SubMeshVertexWeights()
	{
		m_bIsValid = false;
	}

	bool m_bIsValid;
	// map of vertex index and vertex weights
	Map<i32, VertexWeights> m_weights;
};

class AnimationKey
{
public:
	AnimationKey()
	{
		m_time = 0;
		m_values[0] = 0;
		m_values[1] = 0;
		m_values[2] = 0;
		m_values[3] = 0;
		m_tcb.set(0.5f, 0.5f, 0.5f);
		m_easeIn = 0;
		m_easeOut = 0;
	}

	f32 m_time;
	f32 m_values[4];
	Vec3 m_tcb;
	f32 m_easeIn, m_easeOut;
};

class Animation : public CommonInfo
{
public:
	Animation();
	virtual ~Animation();

	String m_nodeName;
	Array<AnimationKey*> m_translationKeys;
	Array<AnimationKey*> m_rotationKeys;
	Array<AnimationKey*> m_scaleKeys;
};

class GeometryBuffer
{
public:
	GeometryBuffer();
	virtual ~GeometryBuffer();

	void free();

	int m_uvSetCount, m_flags;
	Array<Vertex> m_vertices;
	Array<int> m_indices;
};

class MeshPartHull : public NodeTransform
{
public:
	MeshPartHull();
	virtual ~MeshPartHull();

	void copyFromMeshPart(MeshPart* pPart);
	void free();

	Array<Vec3> m_vertices;
	Array<int> m_indices;
};

class MeshCluster
{
public:
	MeshCluster();

	int m_startIndex, m_indexCount, m_primitiveCount;
	GeometryBuffer* m_pGeomBuffer;
	Material* m_pMaterial;
};

class MeshPart: public NodeTransform
{
public:
	MeshPart()
	{
		m_pParent = nullptr;
		m_pMaterial = nullptr;
		m_pHull = nullptr;
	}

	void free();

	String m_name;
	Material* m_pMaterial;
	MeshPart* m_pParent;
	Array<MeshCluster*> m_clusters;
	Array<MeshPart*> m_children;
	MeshPartHull* m_pHull;
};

class Mesh : public CommonInfo
{
public:
	Mesh();
	virtual ~Mesh();

	void free();
	int findGeometryBufferIndex(GeometryBuffer* pBuffer);
	int findHullIndex(MeshPartHull* pHull);

	u32 m_flags;
	bool m_bIsLod;
	Array<Material*> m_materials;
	Array<MeshPart*> m_parts;
	Array<GeometryBuffer*> m_geometryBuffers;
	Matrix m_bindShapeMatrix;
	Array<MeshPartHull*> m_hulls;
	Array<Node> m_bones;
	Array<Animation*> m_boneAnimations;
	Map<int, SubMeshVertexWeights> m_subMeshWeights;
	Array<Mesh*> m_lods;
};

extern void logDebugMatrix(const Matrix& m);
extern void logDebugVec(const Vec3& v);
extern String f2t(f32 aValue);
}