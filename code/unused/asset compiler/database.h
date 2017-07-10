#pragma once
#include "common.h"
#include "../../3rdparty/sqlite/sqlite3.h"

namespace ac
{
struct AssetRecord
{
	ResourceId resId;
	String filename;
	String destFilename;
	DateTime lastWriteTime;
	bool bHasThumbnail;
	bool bIgnoreCompile;
	bool bDevOnly;
	u32 groupId;
	ResourceType type;
	u32 userId;
	u32 bundleId;

	AssetRecord()
		: resId(0)
		, bHasThumbnail(false)
		, bIgnoreCompile(false)
		, bDevOnly(false)
		, groupId(0)
		, type(ResourceTypes::None)
		, userId(0)
		, bundleId(0)
	{}
};

struct AssetDependencyNode
{
	ResourceId resId;
	bool bResolved;
	bool bResolvedTemp;
	Array<AssetDependencyNode*> dependants;
	JsonDocument jsonDoc;
	AssetRecord assetRecord;

	AssetDependencyNode()
		: resId(0)
		, bResolved(false)
		, bResolvedTemp(false)
	{}
};

class AssetDatabase
{
public:
	AssetDatabase();
	~AssetDatabase();

	bool addAsset(const AssetRecord& asset);
	void bulkAddAssets(const Project& project, const String& bundleName, const String& mask, const String& excludeMask);
	void deleteAsset(ResourceId resId, bool bDeleteFileAlso = false);
	AssetRecord findAsset(ResourceId resId);
	void findAssetsForBundle(u32 bundleId, Array<AssetRecord>& outAssets);
	void modifyAsset(const AssetRecord& rec);
	void createCompileTimeDependency(ResourceId resId, ResourceId dependsOn);
	void createRuntimeDependency(ResourceId resId, ResourceId dependsOn);
	void deleteDependenciesOf(ResourceId resId);
	void fillCompileTimeDependenciesOf(ResourceId resId, Array<ResourceId>& deps);
	void fillCompileTimeDependantsFor(ResourceId resId, Array<ResourceId>& dependants);
	void checkForModifiedAssets(Array<ResourceId>& outIds, bool bForceAllModified = false);
	void generateSortedAssetNodes(const Array<ResourceId>& changedAssetIds, Array<AssetDependencyNode*>& sortedNodes, Project& project);
	void markAssetAsModified(ResourceId resId, u32 timestamp);
	void markAssetAsCompiled(ResourceId resId, u32 timestamp);
	bool loadDatabase(const String& projectPath);
	void createTables();
	void consolidate(bool bDeleteMissingAssets = true);

protected:
	void addDependentAssetsRecursive(AssetDependencyNode* pNode, Map<ResourceId, AssetDependencyNode*>& allNodes);
	void visitAssetDependencyNode(AssetDependencyNode* pNode, Array<AssetDependencyNode*>& sortedNodes);

	sqlite3* m_db;
	Map<ResourceId, JsonDocument*> m_assetJsonDocs;
};

}