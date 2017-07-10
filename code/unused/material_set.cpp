#include <stdlib.h>
#include "render/material.h"
#include "render/material_set_data.h"
#include "system/core.h"
#include "system/util.h"
#include "system/logger.h"
#include "system/assert.h"
#include "system/resource_manager.h"
#include "system/file.h"
#include "system/file_manager.h"
#include "render/render_manager.h"

namespace nytro
{
MaterialSetData::MaterialSetData()
{
}

MaterialSetData::~MaterialSetData()
{
}

const char* MaterialSetData::resourceFileExtension()
{
	return "materialset";
}

Resource* MaterialSetData::newResourceInstance()
{
	return new MaterialSetData;
}

bool MaterialSetData::load(const char* pFilename)
{
	unload();
	Resource::load(pFilename);
	File* pFile = files().open(m_fileName.c_str(), File::eMode_BinaryRead);

	if (!pFile)
	{
		N_LOG_ERROR("Could not load material set file: '%s'", m_fileName.c_str());
		return false;
	}

	return loadFromFile(pFile);
}

bool MaterialSetData::loadFromFile(File* pFile)
{
	FileChunkHeader chunk;
	String signature;

	while (!pFile->eof())
	{
		if (!pFile->readChunkHeader(&chunk))
			break;

		N_LOG_DEBUG("Reading chunk id %d", chunk.id);

		switch (chunk.id)
		{
		case eMaterialSetChunk_Header:
			{
				if (chunk.version != MaterialSetData::kFileVersion)
				{
					N_LOG_ERROR("MaterialSet version is different from the material set file: %s, Engine: ver%d File: ver%d",
						m_fileName.c_str(), MaterialSetData::kFileVersion, chunk.version);
					return false;
				}

				*pFile >> signature;

				if (signature != "MATERIALSET")
				{
					N_LOG_ERROR("MaterialSet signature 'MATERIALSET' was not found in the material set file : %s, wrong signature: %s",
						m_fileName.c_str(), signature.c_str());
					return false;
				}

				N_LOG_INFO("Loading material set: '%s'", m_fileName.c_str());
				break;
			}

		case eMaterialSetChunk_MaterialSlots:
			{
				i16 mtlSlotCount;
				*pFile >> mtlSlotCount;

				N_LOG_DEBUG("Reading %d material set slots", mtlSlotCount);

				String mtlFile;

				for (i16 i = 0; i < mtlSlotCount; ++i)
				{
					MaterialSlot slot;
				
					*pFile >> slot.name;
					*pFile >> mtlFile;
					slot.pMaterial = resources().load<Material>(mtlFile.c_str());
					m_materialSlots.append(slot);
				}
				break;
			}
		default:
			{
				N_LOG_DEBUG("Ignoring material set chunk id: %d", chunk.id);
				pFile->ignoreChunk(&chunk);
			}
		}
	}

	N_LOG_DEBUG("End material set load.");
	return true;
}

bool MaterialSetData::unload()
{
	free();

	return true;
}

const Array<MaterialSlot>& MaterialSetData::materialSlots() const
{
	return m_materialSlots;
}

Material* MaterialSetData::findMaterial(const char* pSlotName) const
{
	for (size_t i = 0, iCount = m_materialSlots.count(); i < iCount; ++i)
	{
		if (m_materialSlots[i].name == pSlotName)
		{
			return m_materialSlots[i].pMaterial;
		}
	}

	return nullptr;
}

void MaterialSetData::onDependencyUnload(Resource* pDependencyRes)
{
	if (!pDependencyRes)
	{
		return;
	}

	auto iter = m_materialSlots.begin();

	while (iter != m_materialSlots.end())
	{
		if (pDependencyRes == (*iter).pMaterial)
		{
			N_LOG_DEBUG("Removing dependency material '%s'...", pDependencyRes->fileName().c_str());
			iter->pMaterial = nullptr;
		}

		++iter;
	}
}

void MaterialSetData::free()
{
	for (size_t i = 0, iCount = m_materialSlots.count(); i < iCount; ++i)
	{
		if (!m_materialSlots[i].pMaterial)
		{
			continue;
		}

		resources().release(m_materialSlots[i].pMaterial);
	}
	
	m_materialSlots.clear();
}
}