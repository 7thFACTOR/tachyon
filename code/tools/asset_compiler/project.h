// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/cmdline_arguments.h"
#include "types.h"

namespace ac
{
class Project
{
public:
	Project();
	~Project();

	bool load(const String& projectFilename);
	void importNewAssets(bool forceImport, bool noCompile);
	void importAssets(BundleInfo* bundle, bool forceImport, bool noCompile);
	void importAsset(const String& importFilename, BundleInfo* bundle, bool forceImport, bool noCompile);
	bool generateBundle(BundleInfo* bundle);
	bool saveDatabase(BundleInfo* bundle);
	bool loadDatabase(BundleInfo* bundle);
	bool loadAllBundleDatabases();
	bool saveAllBundleDatabases();
	BundleInfo* getBundleInfoForFilename(const String& filename);
	void getAllAssets(Array<Asset*>& outAllAssets);
	void checkForModifiedAssets(const Array<Asset*>& allAssets, Array<Asset*>& outModifiedAssets, bool forceModified);
	Asset* findAsset(ResourceId resId);
	Asset* findAsset(AssetUuid uuid);
	BuildConfig* findBuildConfig(const String& name);

	String title;
	String description;
	String rootPath;
    bool cleanShippingFolder = true; // will delete the output asset files, because we keep the in the final bundle files
 	BuildConfig* currentBuildConfig;
	JsonDocument projectJsonDoc;
	Array<BuildConfig*> buildConfigs;
	Array<BundleInfo*> bundles;
};

}