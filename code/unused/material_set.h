#pragma once
//*****************************************************************************
// Nytro Game Engine
//
// (C) Copyright 7thFACTOR Entertainment
// All rights reserved
//*****************************************************************************
#include "system/string.h"
#include "system/array.h"
#include "system/defines.h"
#include "system/types.h"
#include "system/resource.h"
#include "system/serialize.h"

namespace nytro
{
class Material;

enum EMaterialSetChunkIds
{
	eMaterialSetChunk_Header,
	eMaterialSetChunk_MaterialSlots
};

struct MaterialSlot
{
	MaterialSlot()
		: pMaterial(nullptr)
	{}
	
	String name;
	String fileName;
	Material* pMaterial;
};

//! This class is a high level surface material, which can be set to submeshes of a mesh
class N_API MaterialSetData : public Resource, public Naming
{
public:
	static const int kFileVersion = 1;

	MaterialSetData();
	virtual ~MaterialSetData();

	static const char* resourceFileExtension();
	static Resource* newResourceInstance();
	//! load the resource, if pFilename is nullptr, will use the m_fileName,
	//! else will set m_fileName to pFilename and use that
	bool load(const char* pFilename = nullptr);
	//! unload the resource
	bool unload();
	void onDependencyUnload(Resource* pDependencyRes);
	bool loadFromFile(File* pFile);

	//! \return the material slots, if any
	const Array<MaterialSlot>& materialSlots() const;
	//! \return the material by its slot name, or nullptr if not found
	Material* findMaterial(const char* pSlotName) const;
	//! free the used resources
	void free();
	
protected:
	Array<MaterialSlot> m_materialSlots;
};
}