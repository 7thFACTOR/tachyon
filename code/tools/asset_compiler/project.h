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
	void importNewAssets(bool forceImport, bool noCompile, const String& mask, const String& excludeMask);
	void importAssets(BundleInfo* bundle, bool forceImport, bool noCompile, const String& mask, const String& excludeMask);
	void importAsset(const String& importFilename, BundleInfo* bundle, bool forceImport, bool noCompile);
	bool generateBundle(BundleInfo* bundle);
	bool saveDatabase(BundleInfo* bundle);
	bool loadDatabase(BundleInfo* bundle);
	bool loadAllBundleDatabases();
	bool saveAllBundleDatabases();
	BundleInfo* getBundleInfoForFilename(const String& filename);
	void resolveAllDependencies();
	void getAllAssets(Array<Asset*>& outAllAssets);
	void checkForModifiedAssets(const Array<Asset*>& allAssets, Array<Asset*>& outModifiedAssets, bool forceModified);
	void sortAssetsByDependencies(const Array<Asset*>& allAssets, const Array<Asset*>& modifiedAssets, Array<Asset*>& outSortedAssets);
	Asset* findAsset(ResourceId resId);
	Asset* findAsset(AssetUuid uuid);
	void visitAsset(Asset* asset, Array<Asset*>& sortedAssets);

	String title;
	String description;
	String rootPath;
	BuildConfig* currentBuildConfig;
	JsonDocument projectJsonDoc;
	Array<BuildConfig*> buildConfigs;
	Array<BundleInfo*> bundles;
};

}