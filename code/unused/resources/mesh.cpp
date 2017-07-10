#include "resources/mesh.h"
#include "render/mesh_part.h"
#include "render/bone.h"
#include "core/core.h"
#include "base/logger.h"
#include "base/assert.h"
#include "core/resource_manager.h"
#include "base/math/conversion.h"
#include "render/geometry_buffer.h"
#include "render/vertex_buffer.h"
#include "render/index_buffer.h"
#include "render/types.h"

namespace engine
{
//! maximum bone influences per vertex
const u32 kMaxBonesPerVertex = 4;
const int kMaxUvSets = 4;

enum EGeomBufferFlag
{
	eGeomBufferFlag_Positions = B_BIT(0),
	eGeomBufferFlag_Normals = B_BIT(1),
	eGeomBufferFlag_Tangents = B_BIT(2),
	eGeomBufferFlag_Bitangents = B_BIT(3),
	eGeomBufferFlag_Colors = B_BIT(4),
	eGeomBufferFlag_TexCoord0 = B_BIT(5),
	eGeomBufferFlag_TexCoord1 = B_BIT(6),
	eGeomBufferFlag_TexCoord2 = B_BIT(7),
	eGeomBufferFlag_TexCoord3 = B_BIT(8),
	eGeomBufferFlag_Weights = B_BIT(9)
};

//---

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	for (size_t i = 0, iCount = m_geometryBuffers.size(); i < iCount; ++i)
	{
		delete m_geometryBuffers[i];
	}

	for (size_t i = 0, iCount = m_bones.size(); i < iCount; ++i)
	{
		delete m_bones[i];
	}

	for (size_t i = 0, iCount = m_parts.size(); i < iCount; ++i)
	{
		delete m_parts[i];
	}

	for (size_t i = 0, iCount = m_hulls.size(); i < iCount; ++i)
	{
		delete m_hulls[i];
	}
}

MeshPart* Mesh::findMeshPartByName(const char* pName) const
{
	for (u32 i = 0, iCount = m_parts.size(); i < iCount; ++i)
	{
		if (m_parts[i]->name == pName)
		{
			return m_parts[i];
		}
	}

	return nullptr;
}

bool Mesh::load(Stream* pStream)
{
	Resource::load(pStream);

	ChunkHeader chunk;
	String signature;
	u32 geomBufferCount = 0, uvSetCount = 0;
	u16 boneCount = 0;
	u32 vertexCount = 0, hullCount = 0, partCount = 0,
		uvCount[kMaxUvSets], primitiveCount, weightCount;
	u8 primitiveType;
	GeometryBuffer* pGeomBuffer = nullptr;
	u16 boneIndex[kMaxBonesPerVertex];
	f32 weight[kMaxBonesPerVertex];
	Map<MeshPart*, u32> partParents;
	Map<MeshPart*, u32> partHullIndices;
	u32 clusterCount = 0;
	f32 angle, ax, ay, az;
	Vec3 vmin, vmax, vec;
	
	while (!pStream->eof())
	{
		if (!pStream->readChunkHeader(&chunk))
			break;

		B_LOG_DEBUG("Reading chunk id %d", chunk.id);

		switch (chunk.id)
		{
		case eMeshChunk_Header:
		{
			if (chunk.version != kMeshFileVersion)
			{
				B_LOG_ERROR("MeshData version is different from the mesh file: Engine: ver%d File: ver%d",
					kMeshFileVersion, chunk.version);
				return false;
			}

			*pStream >> m_meshFlags;
			*pStream >> geomBufferCount;
			*pStream >> partCount;
			*pStream >> hullCount;
			*pStream >> boneCount;
			*pStream >> m_bindShapeMatrix;

			B_LOG_INFO("Header info: GeomBufferCount: %d, BoneCount: %d, PartCount: %d, HullCount: %d",
				geomBufferCount, boneCount, partCount, hullCount);
			break;
		}

		case eMeshChunk_Bones:
			{
				B_LOG_DEBUG("Reading %d bones", boneCount);

				for (u32 i = 0; i < boneCount; ++i)
				{
					SkeletonJoint* pBone = new SkeletonJoint;
					Matrix mtxBindPoseInverse, mtxLocal;

					B_ASSERT(pBone);

					if (!pBone)
					{
						continue;
					}

					String str;

					*pStream >> str;
					pBone->setName(str.c_str());
					*pStream >> str;
					pBone->setParentBoneName(str.c_str());

					*pStream >> mtxBindPoseInverse;
					*pStream >> mtxLocal;
					
					pBone->setBindPoseInverseMatrix(mtxBindPoseInverse);
					pBone->setOffsetMatrix(mtxLocal);
					pBone->setTranslation(mtxLocal.translation());
					pBone->setRotation(toQuat(mtxLocal));
					m_bones.append(pBone);
					B_LOG_DEBUG("    Read bone: %s, parent %s",
						pBone->name().c_str(), pBone->parentBoneName().c_str());
				}
				break;
			}

		case eMeshChunk_GeometryBuffers:
			{
				B_LOG_DEBUG("Reading %d geometry buffers", geomBufferCount);

				for (u32 i = 0; i < geomBufferCount; ++i)
				{
					*pStream >> vertexCount;
					*pStream >> uvSetCount;
					u32 geomFlags;
					*pStream >> geomFlags;

					for (u32 j = 0; j < uvSetCount; ++j)
					{
						uvCount[j] = vertexCount;
					}

					*pStream >> primitiveType;
					*pStream >> primitiveCount;

					B_LOG_DEBUG("    Reading geometry buffer #%d (flags: %d): %d verts, %d uvSets, %d primitives",
						i, geomFlags, vertexCount, uvSetCount, primitiveCount);

					GpuBufferDesc descVb;
					GpuBufferDesc descIb;

					if (geomFlags & eGeomBufferFlag_Positions)
					{
						descVb.addPositionElement();
						B_LOG_DEBUG("Has Pos");
					}

					if (geomFlags & eGeomBufferFlag_Normals)
					{
						descVb.addNormalElement();
						B_LOG_DEBUG("Has Normals");
					}

					if (geomFlags & eGeomBufferFlag_Tangents)
					{
						descVb.addTangentElement();
						B_LOG_DEBUG("Has Tans");
					}

					if (geomFlags & eGeomBufferFlag_Bitangents)
					{
						descVb.addBitangentElement();
						B_LOG_DEBUG("Has Bitans");
					}

					if (geomFlags & eGeomBufferFlag_Colors)
					{
						descVb.addColorElement();
						B_LOG_DEBUG("Has Colors");
					}

					if (geomFlags & eGeomBufferFlag_Weights)
					{
						descVb.addElement(GpuBufferElement::eComponentType_Int8, 1, GpuBufferElement::eType_JointIndex);
						descVb.addElement(GpuBufferElement::eComponentType_Float, 1, GpuBufferElement::eType_JointWeight);
						B_LOG_DEBUG("Has Weights");
					}

					for (u32 j = 0; j < uvSetCount; ++j)
					{
						if (uvCount[j])
						{
							descVb.addElement(GpuBufferElement::eComponentType_Float, 2, GpuBufferElement::eType_TexCoord, j);
							B_LOG_DEBUG("Has UV%d", j);
						}
					}

					descIb.addElement(GpuBufferElement::eComponentType_Int32, 1, GpuBufferElement::eType_Index);

					pGeomBuffer = new GeometryBuffer();
					B_ASSERT(pGeomBuffer);

					if (!pGeomBuffer)
					{
						B_LOG_ERROR("Could not create geometry buffer");
					}
					else
					{
						u8* pVertexData = nullptr;
						u8* pIndexData = nullptr;

						pGeomBuffer->create();
						pGeomBuffer->vertexBuffer()->setupElements(descVb, vertexCount);
						pGeomBuffer->vertexBuffer()->initialize(vertexCount * descVb.stride());
						pGeomBuffer->indexBuffer()->setupElements(descIb, primitiveCount * 3);
						pGeomBuffer->indexBuffer()->initialize(primitiveCount * 3 * sizeof(VertexIndexType));
						pVertexData = (u8*)pGeomBuffer->vertexBuffer()->mapFull();
						pIndexData = (u8*)pGeomBuffer->indexBuffer()->mapFull();
						B_ASSERT_MSG(pVertexData && pIndexData,
										"Vertex and/or index data are nullptr");

						if (!pVertexData || !pIndexData)
						{
							return false;
						}

						// read vertices
						for (u32 j = 0; j < vertexCount; ++j)
						{
							if (geomFlags & eGeomBufferFlag_Positions)
							{
								*pStream >> *((Vec3*)pVertexData);
								pVertexData += sizeof(Vec3);
							}

							if (geomFlags & eGeomBufferFlag_Normals)
							{
								*pStream >> *((Vec3*)pVertexData);
								pVertexData += sizeof(Vec3);
							}

							if (geomFlags & eGeomBufferFlag_Tangents)
							{
								*pStream >> *(Vec3*)pVertexData;
								pVertexData += sizeof(Vec3);
							}

							if (geomFlags & eGeomBufferFlag_Bitangents)
							{
								*pStream >> *(Vec3*)pVertexData;
								pVertexData += sizeof(Vec3);
							}

							if (geomFlags & eGeomBufferFlag_Colors)
							{
								*pStream >> *(Color*)pVertexData;
								pVertexData += sizeof(Color);
							}

							for (u32 k = 0; k < uvSetCount; ++k)
							{
								if (uvCount[k])
								{
									pStream->read(pVertexData, sizeof(f32) * 2);
									pVertexData += sizeof(f32) * 2;
								}
							}

							if (geomFlags & eGeomBufferFlag_Weights)
							{
								// read weights for this vertex
								*pStream >> weightCount;

								B_LOG_DEBUG("Reading weights %d", weightCount);

								for (u32 k = 0; k < kMaxBonesPerVertex; ++k)
								{
									// reset to no bone
									boneIndex[k] = 0;
									weight[k] = 0;
								}

								for (u32 k = 0; k < weightCount; ++k)
								{
									*pStream >> boneIndex[k];
									*pStream >> weight[k];

									B_LOG_DEBUG("Weight %f for bone %d", weight[k], boneIndex[k]);
								}

								for (u32 k = 0; k < kMaxBonesPerVertex; ++k)
								{
									*((f32*)pVertexData) = weight[k];
									pVertexData += sizeof(f32);
								}

								for (u32 k = 0; k < kMaxBonesPerVertex; ++k)
								{
									*((u8*)pVertexData) = boneIndex[k];
									pVertexData += sizeof(u8);
								}
							}
						}

						for (u32 j = 0; j < primitiveCount; ++j)
						{
							pStream->read(pIndexData, sizeof(u32) * 3);
							pIndexData += sizeof(u32) * 3;
						}

						pGeomBuffer->vertexBuffer()->unmap();
						pGeomBuffer->indexBuffer()->unmap();
						m_geometryBuffers.append(pGeomBuffer);
					}
				}

				B_LOG_DEBUG("End read geometry buffers.");

				break;
			}

		case eMeshChunk_Parts:
			{
				partParents.clear();
				partHullIndices.clear();

				B_LOG_DEBUG("Reading %d mesh parts...", partCount);

				for (u32 i = 0; i < partCount; ++i)
				{
					MeshPart* pPart = new MeshPart();

					B_ASSERT(pPart);
					*pStream >> pPart->name;
					*pStream >> partParents[pPart];
					*pStream >> partHullIndices[pPart];
					*pStream >> clusterCount;

					*pStream >> vmin;
					*pStream >> vmax;

					pPart->localBoundingBox.setMin(vmin);
					pPart->localBoundingBox.setMax(vmax);

					*pStream >> pPart->translation;
					*pStream >> pPart->scale;

					*pStream >> angle;
					*pStream >> ax;
					*pStream >> ay;
					*pStream >> az;
					toQuat(angle, ax, ay, az, pPart->rotation);

					B_LOG_DEBUG("    Reading mesh part name: '%s' parentIndex %d, hullIndex: %d, BBox(min[%f,%f,%f] max[%f,%f,%f]) Trans(%f,%f,%f) Scale(%f,%f,%f) Rotation(angle:%f axis:%f,%f,%f)",
								pPart->name.c_str(), partParents[pPart], partHullIndices[pPart],
								vmin.x, vmin.y, vmin.z, vmax.x, vmax.y, vmax.z,
								pPart->translation.x, pPart->translation.y, pPart->translation.z,
								pPart->scale.x, pPart->scale.y, pPart->scale.z,
								angle, ax, ay, az);

					//
					// read clusters of this part
					//
					u32 geometryBufferIndex = 0;

					for (u32 j = 0; j < clusterCount; ++j)
					{
						MeshCluster* pCluster = new MeshCluster();

						*pStream >> geometryBufferIndex;
						pCluster->pGeometryBuffer = geometryBufferIndex >= 0
													&& geometryBufferIndex < m_geometryBuffers.size()
														? m_geometryBuffers[geometryBufferIndex] : nullptr;
						*pStream >> pCluster->indexOffset;
						*pStream >> pCluster->indexCount;
						*pStream >> pCluster->primitiveCount;

						B_LOG_DEBUG("    Cluster#%d: geomBuffIdx: %d, indexOffset: %d, indexCount: %d, primitives: %d",
										j, geometryBufferIndex, pCluster->indexOffset,
										pCluster->indexCount, pCluster->primitiveCount);

						if (pCluster->pGeometryBuffer)
						{
							pPart->clusters.append(pCluster);
						}
						else
						{
							B_LOG_ERROR("    Cluster has no geometry buffer, at index %d, dumped...", j);
						}
					}

					m_parts.append(pPart);
				}

				//
				// solve parts parenting
				//
				for (u32 i = 0, iCount = m_parts.size(); i < iCount; ++i)
				{
					m_parts[i]->pParent =
									(partParents[m_parts[i]] == 0xFFFFFFFF)
										? nullptr : m_parts[partParents[m_parts[i]]];

					if (m_parts[i]->pParent)
					{
						Array<MeshPart*>::Iterator iter = m_parts[i]->pParent->children.find(m_parts[i]);

						if (iter == m_parts[i]->pParent->children.end())
						{
							m_parts[i]->pParent->children.append(m_parts[i]);
						}
					}
				}

				break;
			}

		case eMeshChunk_Hulls:
			{
				u8* pVertexData = nullptr;
				u8* pIndexData = nullptr;
				GeometryBuffer* pHull = nullptr;
				
				for (u32 i = 0; i < hullCount; ++i)
				{
					pVertexData = nullptr;
					pIndexData = nullptr;
					pHull = nullptr;

					*pStream >> vertexCount;
					*pStream >> primitiveCount;

					pHull = new GeometryBuffer();
					B_ASSERT(pHull);

					if (!pHull)
					{
						break;
					}

					GpuBufferDesc descVb;
					GpuBufferDesc descIb;

					descVb.addElement(GpuBufferElement::eComponentType_Float, 3, GpuBufferElement::eType_Position);
					descIb.addElement(GpuBufferElement::eComponentType_Int32, 1, GpuBufferElement::eType_Index);

					//TODO: should we keep this in vbuffers ? needed for debugviz only
					pHull->create();
					pHull->vertexBuffer()->setupElements(descVb, vertexCount);
					pHull->vertexBuffer()->initialize(vertexCount);
					pHull->indexBuffer()->setupElements(descIb, primitiveCount * 3);
					pVertexData = (u8*)pHull->vertexBuffer()->mapFull();
					pIndexData = (u8*)pHull->indexBuffer()->mapFull();
					B_ASSERT_MSG(pVertexData && pIndexData,
								"Hull mesh vertex and/or index data are nullptr");

					// read hull vertex positions
					for (u32 i = 0; i < vertexCount; ++i)
					{
						*pStream >> vec;
						*((Vec3*)pVertexData) = vec;
						pVertexData += sizeof(Vec3);
					}

					// read hull triangle indices
					for (u32 i = 0; i < primitiveCount; ++i)
					{
						pStream->read(pIndexData, sizeof(i32) * 3);
						pIndexData += sizeof(i32) * 3;
					}

					pHull->vertexBuffer()->unmap();
					pHull->indexBuffer()->unmap();
					B_LOG_INFO("    Added mesh hull #%d (%d vertices %d triangles).",
									m_hulls.size(), vertexCount, primitiveCount);
					m_hulls.append(pHull);
				}

				//
				// solve hull ptrs for parts
				//
				if (!m_hulls.isEmpty())
				{
					for (u32 i = 0; i < m_parts.size(); ++i)
					{
						m_parts[i]->pHull = m_hulls[partHullIndices[m_parts[i]]];
					}
				}

				break;
			}

		default:
			{
				B_LOG_DEBUG("Ignoring mesh chunk id: %d", chunk.id);
				pStream->ignoreChunk(&chunk);
			}
		}
	}

	B_LOG_DEBUG("End mesh load.");

	return true;
}

bool Mesh::unload()
{
	for (size_t i = 0, iCount = m_geometryBuffers.size(); i < iCount; ++i)
	{
		delete m_geometryBuffers[i];
	}

	m_geometryBuffers.clear();

	for (size_t i = 0, iCount = m_bones.size(); i < iCount; ++i)
	{
		delete m_bones[i];
	}

	m_bones.clear();

	for (size_t i = 0, iCount = m_parts.size(); i < iCount; ++i)
	{
		delete m_parts[i];
	}

	m_parts.clear();

	for (size_t i = 0, iCount = m_hulls.size(); i < iCount; ++i)
	{
		delete m_hulls[i];
	}

	m_hulls.clear();

	return true;
}

const Array<SkeletonJoint*>&  Mesh::bones() const
{
	return m_bones;
}

const Matrix& Mesh::bindShapeMatrix() const
{
	return m_bindShapeMatrix;
}

const Array<GeometryBuffer*>& Mesh::hulls() const
{
	return m_hulls;
}

void Mesh::setBindShapeMatrix(const Matrix& rValue)
{
	m_bindShapeMatrix = rValue;
}
}