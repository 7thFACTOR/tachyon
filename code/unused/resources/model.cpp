#include <stdlib.h>
#include "resources/model.h"
#include "render/mesh_lod.h"
#include "resources/mesh.h"
#include "render/mesh_part.h"
#include "resources/material.h"
#include "base/logger.h"
#include "core/resource_manager.h"
#include "core/core.h"
#include "base/assert.h"

namespace engine
{
B_REGISTER_DERIVED_TAGGED_CLASS(ModelData, Resource, ResourceTypes::Model);

const int kModelFileVersion = 1;

ModelData::ModelData()
{
}

ModelData::~ModelData()
{
}

bool ModelData::load(Stream* pStream)
{
	Resource::load(pStream);
	ChunkHeader chunk;
	String signature, partName;
	ResourceId physMtlRes;
	ResourceId mtlId;

	while (!pStream->eof())
	{
		if (!pStream->readChunkHeader(&chunk))
			break;

		B_LOG_DEBUG("Reading chunk id %d", chunk.id);

		switch (chunk.id)
		{
		case eModelChunk_Header:
			{
				if (chunk.version != kModelFileVersion)
				{
					B_LOG_ERROR("ModelData version is different from the model file: Engine: ver%d File: ver%d",
						kModelFileVersion, chunk.version);
					return false;
				}

				*pStream >> physMtlRes;
				break;
			}

		case eModelChunk_Meshes:
			{
				i16 meshCount = 0;
				ResourceId meshRes;
				String meshName;

				*pStream >> meshCount;

				B_LOG_DEBUG("Reading %d meshes", meshCount);
				
				for (u32 i = 0; i < meshCount; ++i)
				{
					*pStream >> meshName;
					*pStream >> meshRes;
					resources().load(meshRes);
					m_usedMeshes[meshName] = meshRes;
				}

				break;
			}

		case eModelChunk_LODs:
			{
				i16 lodCount = 0;

				*pStream >> lodCount;
				B_LOG_DEBUG("Reading %d lods", lodCount);

				for (u32 i = 0; i < lodCount; ++i)
				{
					MeshLevelOfDetail* pMLOD = new MeshLevelOfDetail();
					i16 partCount = 0;

					*pStream >> pMLOD->m_fromDistance;
					*pStream >> pMLOD->m_meshName;
					*pStream >> partCount;

					TMeshNameMeshDataMap::Iterator iter = m_usedMeshes.find(pMLOD->m_meshName);

					ResourceId mesh = kInvalidResourceId;
					
					if (iter != m_usedMeshes.end())
					{
						mesh = m_usedMeshes[pMLOD->m_meshName];
					}
					else
					{
						B_LOG_DEBUG("Could not find '%s' stock mesh", pMLOD->m_meshName.c_str());
						delete pMLOD;
						return false;
					}
					
					pMLOD->m_mesh = mesh;
					
					for (i16 j = 0; j < partCount; ++j)
					{
						*pStream >> partName;
						
						if (mesh == kInvalidResourceId)
						{
							B_LOG_DEBUG("Could not find mesh named %s", pMLOD->m_meshName.c_str());
							delete pMLOD;
							return false;
						}

						MeshLevelOfDetail::MeshPartInfo partInfo;

						*pStream >> mtlId;
						partInfo.m_partName = partName;
						partInfo.m_defaultMaterial = mtlId;

						if (!partInfo.m_defaultMaterial)
						{
							partInfo.m_defaultMaterial = resources().id(E_MISSING_MATERIAL_FILENAME);
						}

						i16 clusterCount = 0;

						*pStream >> clusterCount;

						//TODO:
						//for (i16 k = 0; k < clusterCount; ++k)
						//{
						//	if (k >= pPart->clusters.size())
						//	{
						//		break;
						//	}

						//	MeshCluster* pCluster = pPart->clusters[k];

						//	if (!pCluster)
						//	{
						//		N_LOG_DEBUG("Could not find mesh cluster at index %d", k);
						//		delete pMLOD;
						//		delete pFile;
						//		return false;
						//	}

						//	*pStream >> mtlFilename;

						//	Material* pMtl = resources().load<Material>(mtlFilename.c_str());

						//	if (!pMtl)
						//	{
						//		partInfo.m_clusters[pCluster] = resources().load<Material>(E_MISSING_MATERIAL_FILENAME);
						//	}

						//	partInfo.m_clusters[pCluster] = pMtl;
						//}
						//
						//pMLOD->m_meshParts[pPart] = partInfo;
					}

					m_levelsOfDetail.append(pMLOD);
				}

				B_LOG_DEBUG("End read lods.");

				break;
			}

		default:
			{
				B_LOG_DEBUG("Ignoring model chunk id: %d", chunk.id);
				pStream->ignoreChunk(&chunk);
			}
		}
	}

	B_LOG_DEBUG("End model load.");

	return true;
}

bool ModelData::unload()
{
	for (u32 i = 0, iCount = m_levelsOfDetail.size(); i < iCount; ++i)
	{
		delete m_levelsOfDetail[i];
	}

	m_levelsOfDetail.clear();
	m_usedMeshes.clear();

	return true;
}

const Array<MeshLevelOfDetail*>& ModelData::levelsOfDetail() const
{
	return m_levelsOfDetail;
}
}