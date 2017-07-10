#include "common.h"
#include "mesh_processor.h"
#include "base/math/conversion.h"
#include "base/math/util.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/json.h"
#include "base/util.h"
#include "base/math/color.h"

namespace ac
{
MeshProcessor::MeshProcessor()
{
}

MeshProcessor::~MeshProcessor()
{
	free();
}

void MeshProcessor::free()
{
	m_mesh.free();
}

const char* MeshProcessor::supportedFileExtensions() const
{
	return ".mesh";
}

bool MeshProcessor::process(
		const char* pSrcFilename,
		Stream* pStream,
		const Project& project)
{
	String str;
	String filename;
	//FileWriter file;

	//extractFileName(pSrcFilename, str);
	//filename = mergePathFile(pDestPath, str.c_str());

	//if (isFilesLastTimeSame(pSrcFilename, filename.c_str()))
	//{
	//	B_LOG_DEBUG("Skipping %s", pSrcFilename);
	//	return true;
	//}

	//if (!loadMesh(pSrcFilename))
	//{
	//	return false;
	//}

	//if (file.openFile(filename.c_str()))
	//{
	//	E_LOG_VERBOSE("Writing mesh file: '%s'", filename.c_str());

	//	//
	//	// write header chunk
	//	//
	//	file.beginChunk(eMeshChunk_Header, 1);
	//	file.writeString("MESH");
	//	file.writeInt32(0);// mesh flags
	//	file.writeInt32(m_mesh.m_geometryBuffers.size());
	//	file.writeInt32(m_mesh.m_parts.size());
	//	file.writeInt32(m_mesh.m_hulls.size());
	//	file.writeInt16(m_mesh.m_bones.size());
	//	file.write(&m_mesh.m_bindShapeMatrix.m[0], sizeof(f32) * 16);
	//	file.endChunk();

	//	E_LOG_VERBOSE("Writing bones for mesh: '%s'", filename.c_str());

	//	//
	//	// write bones, if any
	//	//
	//	if (!m_mesh.m_bones.isEmpty())
	//	{
	//		E_LOG_VERBOSE("Writing %d bones...\n", m_mesh.m_bones.size());
	//		file.beginChunk(eMeshChunk_Bones, 1);

	//		for (unsigned int j = 0; j < m_mesh.m_bones.size(); j++)
	//		{
	//			// name
	//			// parent name
	//			// bind pose inv mtx
	//			// bone mtx

	//			Node& rBone = m_mesh.m_bones[j];

	//			E_LOG_VERBOSE("Writing bone info: '%s' parent '%s'", rBone.m_name.c_str(), rBone.m_parentName.c_str());
	//			file.writeString(rBone.m_name.c_str());
	//			file.writeString(rBone.m_parentName.c_str());

	//			Matrix  mtxBindPoseInverse = rBone.m_bindPoseInverse;
	//			Matrix  mtxBoneLocal = rBone.m_localTransform;

	//			E_LOG_VERBOSE("Bind pose inverse matrix:");
	//			logDebugMatrix(mtxBindPoseInverse);
	//			E_LOG_VERBOSE("Bone local matrix:");
	//			logDebugMatrix(mtxBoneLocal);

	//			file.write((void*)&mtxBindPoseInverse.m[0][0], sizeof(f32) * 16);
	//			file.write((void*)&mtxBoneLocal.m[0][0], sizeof(f32) * 16);
	//		}

	//		file.endChunk();
	//	}

	//	//
	//	// write geometry buffers
	//	//

	//	if (!m_mesh.m_geometryBuffers.isEmpty())
	//	{
	//		E_LOG_VERBOSE("Writing %d geom buffers", m_mesh.m_geometryBuffers.size());
	//		file.beginChunk(eMeshChunk_GeometryBuffers, 1);

	//		for (size_t j = 0; j < m_mesh.m_geometryBuffers.size(); j++)
	//		{
	//			GeometryBuffer* pGeomBuffer = m_mesh.m_geometryBuffers[j];
	//			size_t vertexCount = pGeomBuffer->m_vertices.size();

	//			E_LOG_VERBOSE("Writing geom buffer: %d vertices %d indices", vertexCount, pGeomBuffer->m_indices.size());

	//			file.writeInt32(vertexCount);
	//			file.writeInt32(pGeomBuffer->m_uvSetCount);

	//			#define HAS_POSITIONS (1<<0)
	//			#define HAS_NORMALS (1<<1)
	//			#define HAS_TANGENTS (1<<2)
	//			#define HAS_BITANGENTS (1<<3)
	//			#define HAS_COLORS (1<<4)
	//			#define HAS_UVSET0 (1<<5)
	//			#define HAS_UVSET1 (1<<6)
	//			#define HAS_UVSET2 (1<<7)
	//			#define HAS_UVSET3 (1<<8)
	//			#define HAS_WEIGHTS (1<<9)

	//			file.writeInt32(
	//				HAS_POSITIONS | HAS_NORMALS
	//				| HAS_TANGENTS | HAS_BITANGENTS 
	//				| HAS_COLORS | HAS_UVSET0 | HAS_UVSET1 | HAS_UVSET2 | HAS_UVSET3 
	//				| (m_mesh.m_bones.isEmpty() ? 0 : HAS_WEIGHTS));

	//			// primitive type: 0 - TRIANGLES
	//			file.writeInt8(0);

	//			// primitive count
	//			file.writeInt32(pGeomBuffer->m_indices.size() / 3);

	//			// save vertices
	//			for (size_t k = 0; k < vertexCount; k ++)
	//			{
	//				Vertex& vtx = pGeomBuffer->m_vertices[k];

	//				file.writeFloat(vtx.position.x);
	//				file.writeFloat(vtx.position.y);
	//				file.writeFloat(vtx.position.z);

	//				file.writeFloat(vtx.normal.x);
	//				file.writeFloat(vtx.normal.y);
	//				file.writeFloat(vtx.normal.z);

	//				file.writeFloat(vtx.tangent.x);
	//				file.writeFloat(vtx.tangent.y);
	//				file.writeFloat(vtx.tangent.z);

	//				file.writeFloat(vtx.bitangent.x);
	//				file.writeFloat(vtx.bitangent.y);
	//				file.writeFloat(vtx.bitangent.z);

	//				file.writeFloat(vtx.color.r);
	//				file.writeFloat(vtx.color.g);
	//				file.writeFloat(vtx.color.b);
	//				file.writeFloat(vtx.color.a);

	//				for (u32 l = 0; l < pGeomBuffer->m_uvSetCount; ++l)
	//				{
	//					file.writeFloat(vtx.uv[l].u);
	//					file.writeFloat(vtx.uv[l].v);
	//				}

	//				// save weights
	//				if (!m_mesh.m_bones.isEmpty())
	//				{
	//					if (m_mesh.m_subMeshWeights[j].m_bIsValid)
	//					{
	//						file.writeInt16(m_mesh.m_subMeshWeights[j].m_weights[k].m_boneWeights.size());

	//						Map<int, f32>::Iterator iter = m_mesh.m_subMeshWeights[j].m_weights[k].m_boneWeights.begin();

	//						while (iter != m_mesh.m_subMeshWeights[j].m_weights[k].m_boneWeights.end())
	//						{
	//							file.writeInt16(iter->key);
	//							file.writeFloat(iter->value);
	//							E_LOG_VERBOSE("Saving weight idx: %d vertex: %d weight: %f", k, iter->key, iter->value);
	//							++iter;
	//						}
	//					}
	//					else
	//					{
	//						// no bone influences, zero count
	//						file.writeInt16(0);
	//					}
	//				}
	//			}

	//			// write faces
	//			for (size_t k = 0; k < pGeomBuffer->m_indices.size(); k++)
	//			{
	//				file.writeInt32(pGeomBuffer->m_indices[k]);
	//			}
	//		}

	//		file.endChunk(); // geom buffers
	//	}
	//	
	//	//
	//	// mesh parts
	//	//
	//	if (!m_mesh.m_parts.isEmpty())
	//	{
	//		E_LOG_VERBOSE("Writing mesh parts chunk...");
	//		file.beginChunk(eMeshChunk_Parts, 1);

	//		Map<MeshPart*, int> partIndexMap;
	//		Map<MeshPartHull*, int> hullIndexMap;

	//		for (size_t j = 0, jCount = m_mesh.m_parts.size(); j < jCount; ++j)
	//		{
	//			partIndexMap[m_mesh.m_parts[j]] = j;
	//		}

	//		for (size_t j = 0, jCount = m_mesh.m_hulls.size(); j < jCount; ++j)
	//		{
	//			hullIndexMap[m_mesh.m_hulls[j]] = j;
	//		}

	//		for (size_t j = 0, jCount = m_mesh.m_parts.size(); j < jCount; ++j)
	//		{
	//			MeshPart* pPart = m_mesh.m_parts[j];

	//			E_LOG_VERBOSE("Writing mesh part: '%s' with %d clusters", pPart->m_name.c_str(), pPart->m_clusters.size());

	//			file.writeString(pPart->m_name.c_str());
	//			file.writeInt32(pPart->m_pParent ? partIndexMap[pPart->m_pParent] : 0xFFFFFFFF);
	//			file.writeInt32(pPart->m_pHull ? hullIndexMap[pPart->m_pHull] : 0xFFFFFFFF);
	//			file.writeInt32(pPart->m_clusters.size());

	//			file.writeFloat(pPart->m_bbox.minCorner().x);
	//			file.writeFloat(pPart->m_bbox.minCorner().y);
	//			file.writeFloat(pPart->m_bbox.minCorner().z);

	//			file.writeFloat(pPart->m_bbox.maxCorner().x);
	//			file.writeFloat(pPart->m_bbox.maxCorner().y);
	//			file.writeFloat(pPart->m_bbox.maxCorner().z);

	//			file.writeFloat(pPart->m_translation.x);
	//			file.writeFloat(pPart->m_translation.y);
	//			file.writeFloat(pPart->m_translation.z);

	//			file.writeFloat(pPart->m_scale.x);
	//			file.writeFloat(pPart->m_scale.y);
	//			file.writeFloat(pPart->m_scale.z);

	//			AngleAxis aa;

	//			toAngleAxis(pPart->m_rotation, aa);
	//			file.writeFloat(aa.angle);
	//			file.writeFloat(aa.axis.x);
	//			file.writeFloat(aa.axis.y);
	//			file.writeFloat(aa.axis.z);

	//			// write clusters
	//			for (size_t k = 0, kCount = pPart->m_clusters.size(); k < kCount; ++k)
	//			{
	//				file.writeInt32(m_mesh.findGeometryBufferIndex(pPart->m_clusters[k]->m_pGeomBuffer));
	//				file.writeInt32(pPart->m_clusters[k]->m_startIndex);
	//				file.writeInt32(pPart->m_clusters[k]->m_indexCount);
	//				file.writeInt32(pPart->m_clusters[k]->m_primitiveCount);
	//			}
	//		}

	//		file.endChunk(); // parts
	//	}

	//	//
	//	// save hulls
	//	//
	//	E_LOG_VERBOSE("Writing mesh part hulls chunk...");

	//	if (!m_mesh.m_hulls.isEmpty())
	//	{
	//		file.beginChunk(eMeshChunk_Hulls, 1);

	//		for (size_t j = 0; j < m_mesh.m_hulls.size(); ++j)
	//		{
	//			MeshPartHull* pHull = m_mesh.m_hulls[j];

	//			E_LOG_VERBOSE("Saving hull #%d (%d verts %d indices) ...", j,
	//				pHull->m_vertices.size(),
	//				pHull->m_indices.size());
	//			// hull header
	//			file.writeInt32(pHull->m_vertices.size());
	//			file.writeInt32(pHull->m_indices.size() / 3);

	//			// vertices
	//			for (size_t k = 0; k < pHull->m_vertices.size(); ++k)
	//			{
	//				file.writeFloat(pHull->m_vertices[k].x * pHull->m_scale.x);
	//				file.writeFloat(pHull->m_vertices[k].y * pHull->m_scale.y);
	//				file.writeFloat(pHull->m_vertices[k].z * pHull->m_scale.z);
	//			}

	//			// indices, triangles
	//			for (size_t k = 0; k < pHull->m_indices.size(); ++k)
	//			{
	//				file.writeInt32(pHull->m_indices[k]);
	//			}
	//		}

	//		file.endChunk(); // hulls
	//	}

	//	file.closeFile();
	//}
	//else
	//{
	//	B_LOG_ERROR("Could not write mesh file: %s", filename.c_str());
	//	return false;
	//}
	//
	//E_LOG_VERBOSE("Wrote mesh file successfully: %s", filename.c_str());
	//setSameFileTime(pSrcFilename, filename.c_str());

	return true;
}

void fastExplodeString(const char* str, Array<f32>& values, char separator)
{
	const i32 kMaxNumSize = 50;
	char number[kMaxNumSize];
	u32 numberPos = 0;
	u32 stringPos = 0;
	char chr;
	u32 strLen = strlen(str);

	values.clear();
	values.useGrowSize(1000);

	while (stringPos <= strLen)
	{
		chr = str[stringPos];
		
		if (chr != 0 && chr != separator && numberPos < kMaxNumSize)
		{
			number[numberPos++] = chr;
		}
		else
		{
			number[numberPos] = 0;
			numberPos = 0;
			values.append(atof(number));
		}

		++stringPos;
	}
}

bool MeshProcessor::loadMesh(const char* pFilename)
{
	JsonDocument doc;

	if (!doc.loadAndParse(pFilename))
	{
		return false;
	}

	JsonNode* jsnGeomBuffers = doc.findNodeByPath("geomBuffers");
	JsonNode* jsnParts = doc.findNodeByPath("parts");

	if (!jsnGeomBuffers || !jsnParts)
	{
		B_LOG_ERROR("No geom buffers or parts");
		return false;
	}

	char arrayValuesSeparator = ';';
	Array<f32> values;

	for (size_t i = 0; i < jsnGeomBuffers->arrayValues().size(); ++i)
	{
		JsonNode* jsnGB = jsnGeomBuffers->arrayValues().at(i)->asNode();

		if (!jsnGB)
		{
			B_LOG_ERROR("Expected object node in geometry buffer array");
			continue;
		}

		GeometryBuffer* pGB = new GeometryBuffer();
		size_t vindex = 0;

		pGB->m_uvSetCount = jsnGB->valueOf("uvSets").asNode()->arrayValues().size();

		// indices
		fastExplodeString(jsnGB->valueOf("indices").asString(), values, arrayValuesSeparator);
		pGB->m_indices.resize(values.size());
		B_LOG_DEBUG("Indices: %d", pGB->m_indices.size());

		for (size_t j = 0, jCount = values.size(); j < jCount; ++j)
		{
			pGB->m_indices[vindex++] = (int)values[j];
		}

		vindex = 0;

		// positions
		fastExplodeString(jsnGB->valueOf("positions").asString(), values, arrayValuesSeparator);
		pGB->m_vertices.resize(values.size() / 3);
		B_LOG_DEBUG("Vertices: %d", pGB->m_vertices.size());

		for (size_t j = 0, jCount = values.size(); j < jCount; j += 3)
		{
			pGB->m_vertices[vindex++].position.set(
				values[j], values[j + 1], values[j + 2]);
		}
		
		vindex = 0;

		// normals
		fastExplodeString(jsnGB->valueOf("normals").asString(), values, arrayValuesSeparator);
		B_ASSERT(pGB->m_vertices.size() == values.size() / 3);
		
		for (size_t j = 0, jCount = values.size(); j < jCount; j += 3)
		{
			pGB->m_vertices[vindex++].normal.set(values[j], values[j + 1], values[j + 2]);
		}

		vindex = 0;

		// tangents
		fastExplodeString(jsnGB->valueOf("tangents").asString(), values, arrayValuesSeparator);
		B_ASSERT(pGB->m_vertices.size() == values.size() / 3);

		for (size_t j = 0, jCount = values.size(); j < jCount; j += 3)
		{
			pGB->m_vertices[vindex++].tangent.set(values[j], values[j + 1], values[j + 2]);
		}

		vindex = 0;

		// bitangents
		fastExplodeString(jsnGB->valueOf("bitangents").asString(), values, arrayValuesSeparator);
		B_ASSERT(pGB->m_vertices.size() == values.size() / 3);

		for (size_t j = 0, jCount = values.size(); j < jCount; j += 3)
		{
			pGB->m_vertices[vindex++].bitangent.set(values[j], values[j + 1], values[j + 2]);
		}

		vindex = 0;

		// colors
		fastExplodeString(jsnGB->valueOf("colors").asString(), values, arrayValuesSeparator);
		B_ASSERT(pGB->m_vertices.size() == values.size() / 4);

		for (size_t j = 0, jCount = values.size(); j < jCount; j += 4)
		{
			pGB->m_vertices[vindex++].color.set(values[j], values[j + 1], values[j + 2], values[j + 3]);
		}

		vindex = 0;

		// uvSets
		for (int uvs = 0; uvs < pGB->m_uvSetCount; ++uvs)
		{
			fastExplodeString(jsnGB->valueOf("uvSets").asNode()->arrayValues().at(uvs)->asString(),
				values, arrayValuesSeparator);
			B_ASSERT(pGB->m_vertices.size() == values.size() / 2);

			for (size_t j = 0, jCount = values.size(); j < jCount; j += 2)
			{
				pGB->m_vertices[vindex++].uv[uvs].u = values[j];
				pGB->m_vertices[vindex++].uv[uvs].v = values[j + 1];
			}

			vindex = 0;
		}

		m_mesh.m_geometryBuffers.append(pGB);
	}

	Map<String, String> partParentNameMap;
	Map<String, MeshPart*> namePartMap;

	// parts
	for (size_t i = 0; i < jsnParts->arrayValues().size(); ++i)
	{
		JsonNode* jsnPart = jsnParts->arrayValues().at(i)->asNode();

		if (!jsnPart)
		{
			continue;
		}

		if (!jsnPart->isObject())
		{
			continue;
		}

		JsonNode* jsnClusters = jsnPart->valueOf("clusters").asNode();

		if (!jsnClusters)
		{
			continue;
		}

		MeshPart* pPart = new MeshPart();

		pPart->m_name = jsnPart->valueOf("name").asString();
		namePartMap[pPart->m_name] = pPart;
		partParentNameMap[pPart->m_name] = jsnPart->valueOf("parent").asString();
		pPart->m_translation = toVec3(jsnPart->valueOf("translation").asString());
		pPart->m_rotation = toQuat(jsnPart->valueOf("rotation").asString());
		pPart->m_scale = toVec3(jsnPart->valueOf("scale").asString());
		pPart->m_bindPoseInverse = toMatrix(jsnPart->valueOf("bindPoseInverseMatrix").asString());
		pPart->m_bbox = toBBox(jsnPart->valueOf("boundingBox").asString());
		pPart->m_clusters.reserve(jsnClusters->arrayValues().size());

		for (size_t j = 0, jCount = jsnClusters->arrayValues().size(); j < jCount; ++j)
		{
			JsonNode* jsnCluster = jsnClusters->arrayValues().at(j)->asNode();

			if (!jsnCluster)
			{
				continue;
			}

			if (!jsnCluster->isObject())
			{
				continue;
			}

			MeshCluster* pCluster = new MeshCluster();

			pCluster->m_indexCount = jsnCluster->valueOf("indexCount").asInt();
			pCluster->m_primitiveCount = jsnCluster->valueOf("primitiveCount").asInt();
			pCluster->m_startIndex = jsnCluster->valueOf("startIndex").asInt();
			u32 gbindex = jsnCluster->valueOf("geomBufferIndex").asInt();

			if (B_INBOUNDS(gbindex, 0, m_mesh.m_geometryBuffers.size()))
			{
				pCluster->m_pGeomBuffer = m_mesh.m_geometryBuffers[gbindex];
			}
			else
			{
				B_LOG_ERROR("Geometry buffer index out of bounds! %d from %d max", gbindex, m_mesh.m_geometryBuffers.size());
			}

			pPart->m_clusters.append(pCluster);
		}
		
		m_mesh.m_parts.append(pPart);
	}

	// solve parenting
	for (size_t i = 0; i < partParentNameMap.size(); ++i)
	{
		// search for parent
		auto iter = namePartMap.find(partParentNameMap.valueAt(i));

		if (iter != namePartMap.end())
		{
			auto iterKid = namePartMap.find(partParentNameMap.keyAt(i));
			iterKid->value->m_pParent = iter->value;
			iter->value->m_children.appendUnique(iterKid->value);
		}
	}

	return true;
}
}