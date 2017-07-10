#pragma once
#include "../common.h"
#include "../asset_processor.h"
#include "base/math/color.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/matrix.h"
#include "base/math/bbox.h"
#include "base/array.h"
#include "base/string.h"
#include "base/map.h"
#include "render/texcoord.h"

namespace ac
{
using namespace base;
struct MeshPart;

const u32 kMaxTexCoordSets = 4;

struct Vertex
{
	Vec3 position, normal, tangent, bitangent;
	Color color;
	TexCoord uv[kMaxTexCoordSets];
};

class ImportMeshInfo
{
public:
	String name;
	String parentName;
	String dccTool;
	String author;
	String comments;
};

struct ImportMeshTransform
{
public:
	ImportMeshTransform()
		: scale(1, 1, 1)
	{
	}

	Vec3 translation;
	Quat rotation;
	Vec3 scale;
	BBox bbox;
	Matrix bindPoseInverse, localTransform;
};

class ImportMesh : public ImportMeshInfo, public NodeTransform
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

typedef Map<u32/*joint index*/, f32/*weight*/> VertexWeights;

struct MeshClusterVertexWeights
{
	MeshClusterVertexWeights()
	{
		bIsValid = false;
	}

	bool bIsValid;
	Map<u32/*vertex index*/, VertexWeights> weights;
};

struct AnimationKey
{
	AnimationKey()
	{
		time = 0;
		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		tcb.set(0.5f, 0.5f, 0.5f);
		easeIn = 0;
		easeOut = 0;
	}

	f32 time;
	f32 values[4];
	Vec3 tcb;
	f32 easeIn, easeOut;
};

struct Animation
{
	Animation();
	virtual ~Animation();

	String nodeName;
	Array<AnimationKey*> translationKeys;
	Array<AnimationKey*> rotationKeys;
	Array<AnimationKey*> scaleKeys;
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