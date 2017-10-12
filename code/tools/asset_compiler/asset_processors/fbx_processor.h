// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "../common.h"
#include "../asset_processor.h"
#include "mesh_util.h"
#include "fbx_importer.h"

namespace ac
{
class FbxProcessor : public AssetProcessor
{
public:
	FbxProcessor();
	~FbxProcessor();

	bool import(const String& importFilename, JsonDocument& assetCfg) override;
	bool process(Asset& asset, JsonDocument& assetCfg) override;

protected:
	bool importMeshes(FbxFileImporter& fbxImporter, const String& basePath, JsonDocument& assetCfg);
	bool importAnimations(FbxFileImporter& fbxImporter, const String& basePath, JsonDocument& assetCfg);
	bool importMaterials(FbxFileImporter& fbxImporter, const String& basePath, JsonDocument& assetCfg);
};

}