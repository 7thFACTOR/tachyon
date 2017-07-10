#include "common.h"
#include "mesh_processor.h"
#include "base/math/conversion.h"
#include "base/math/util.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/json.h"
#include "base/util.h"

namespace ac
{
void logDebugMatrix(const Matrix& m)
{
	B_LOG_INFO("(%f %f %f %f)", m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3]);
	B_LOG_INFO("(%f %f %f %f)", m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3]);
	B_LOG_INFO("(%f %f %f %f)", m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3]);
	B_LOG_INFO("(%f %f %f %f)", m.m[3][0], m.m[3][1], m.m[3][2], m.m[3][3]);
	B_LOG_INFO("-------------------");
}

void logDebugVec(const Vec3& v)
{
	B_LOG_INFO("V(%f %f %f)", v.x, v.y, v.z);
	B_LOG_INFO("-------------------");
}

String f2t(f32 aValue)
{
	String str;

	if (fabs(aValue) <= 0.0000001f)
	{
		formatString(str, "0");
		return str;
	}

	int ival = (int)aValue;
	f32 frac = aValue - (f32)ival;
	
	if (fabs(frac) <= 0.0000001f)
	{
		formatString(str, "%d", ival);
		return str;
	}

	formatString(str, "%f", aValue);
	return str;
}

//---

GeometryBuffer::GeometryBuffer()
{
	m_uvSetCount = kMaxUvSets;
	m_flags = 0;
}

GeometryBuffer::~GeometryBuffer()
{
}

void GeometryBuffer::free()
{
}

//---

MeshCluster::MeshCluster()
{
	m_startIndex = m_indexCount = m_primitiveCount = 0;
	m_pGeomBuffer = nullptr;
	m_pMaterial = nullptr;
}

//---

Mesh::Mesh()
{
	m_bIsLod = false;
}

Mesh::~Mesh()
{
	free();
}

void Mesh::free()
{
	for (size_t i = 0; i < m_geometryBuffers.size(); ++i)
	{
		delete m_geometryBuffers[i];
	}

	for (size_t i = 0; i < m_materials.size(); ++i)
	{
		delete m_materials[i];
	}

	for (size_t i = 0; i < m_boneAnimations.size(); ++i)
	{
		delete m_boneAnimations[i];
	}

	m_boneAnimations.clear();
	m_subMeshWeights.clear();
	m_geometryBuffers.clear();
	m_parts.clear();
	m_materials.clear();
}

int Mesh::findGeometryBufferIndex(GeometryBuffer* pBuffer)
{
	for (size_t i = 0, iCount = m_geometryBuffers.size(); i < iCount; ++i)
	{
		if (pBuffer == m_geometryBuffers[i])
		{
			return i;
		}
	}

	return INT_MAX;
}

int Mesh::findHullIndex(MeshPartHull* pHull)
{
	for (size_t i = 0, iCount = m_hulls.size(); i < iCount; ++i)
	{
		if (pHull == m_hulls[i])
		{
			return i;
		}
	}

	return INT_MAX;
}

//---

Animation::Animation()
{
}

Animation::~Animation()
{
}

//---

void MeshPart::free()
{
	for (size_t i = 0; i < m_clusters.size(); ++i)
	{
		delete m_clusters[i];
	}

	m_clusters.clear();
}

//---

Node::Node()
{
}

Node::Node(const Node& rOther)
{
	*this = rOther;
}

Node& Node::operator = (const Node& rOther)
{
	this->m_name = rOther.m_name;
	this->m_parentName = rOther.m_parentName;
	this->m_dccTool = rOther.m_dccTool;
	this->m_author = rOther.m_author;
	this->m_date = rOther.m_date;
	this->m_comments = rOther.m_comments;
	this->m_translation = rOther.m_translation;
	this->m_rotation = rOther.m_rotation;
	this->m_scale = rOther.m_scale;
	this->m_bindPoseInverse = rOther.m_bindPoseInverse;
	this->m_localTransform = rOther.m_localTransform;
	//TODO: this->m_box = Box( rOther.m_box );

	return *this;
}

Node::~Node()
{
	//TODO: delete children
}

//---
MeshPartHull::MeshPartHull()
{
}

MeshPartHull::~MeshPartHull()
{
}

void MeshPartHull::copyFromMeshPart(MeshPart* pPart)
{
	if (pPart->m_clusters.isEmpty())
	{
		B_LOG_ERROR("Mesh part considered hull has no clusters!");
		return;
	}

	if (pPart->m_clusters.size() > 1)
	{
		B_LOG_WARNING("Mesh part considered hull has too many clusters, only one accepted");
	}

	*((NodeTransform*)this) = *((NodeTransform*)pPart);

	m_indices = pPart->m_clusters[0]->m_pGeomBuffer->m_indices;
	m_vertices.clear();

	for (size_t i = 0, iCount = pPart->m_clusters[0]->m_pGeomBuffer->m_vertices.size(); i < iCount; ++i)
	{
		Vertex& vtx = pPart->m_clusters[0]->m_pGeomBuffer->m_vertices[i];
		m_vertices.append(vtx.position);
	}
}

void MeshPartHull::free()
{
	m_indices.clear();
	m_vertices.clear();
}
}