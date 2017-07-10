#include "database.h"
#include "asset_processor_registry.h"
#include "project.h"

namespace ac
{
AssetDatabase::AssetDatabase()
{
}

AssetDatabase::~AssetDatabase()
{
}

bool AssetDatabase::addAsset(const AssetRecord& asset)
{
	String str;

	AssetRecord rec = findAsset(asset.resId);

	if (rec.resId != 0)
	{
		B_LOG_ERROR("Cannot add asset, already added id: %u filename: %s", asset.resId, asset.filename.c_str());
		return false;
	}

	asset.lastWriteTime.debug();

	formatString(str, "INSERT INTO assets (resId, filename, destFilename, lastWriteTime,"
		"hasThumbnail, ignoreCompile, devOnly, typeId, groupId, lastUserId, bundleId)"
		" VALUES (%d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d)",
		asset.resId, asset.filename.c_str(), asset.destFilename.c_str(), asset.lastWriteTime.toUnixTime(),
		asset.bHasThumbnail, asset.bIgnoreCompile, asset.bDevOnly, asset.type,
		asset.groupId, asset.userId, asset.bundleId);

	sqlite3_exec(m_db, str.c_str(), 0, 0, 0);

	return true;
}

void AssetDatabase::bulkAddAssets(const Project& project, const String& bundleName, const String& mask, const String& excludeMask)
{
	String ext;
	Array<FoundFileInfo> files;
	String destFolder, filenameMask;
	String srcAssetFile;
	String outAssetFile;
	String assetFilenameOnly;

	for (size_t bundleIndex = 0; bundleIndex < project.bundles.size(); bundleIndex++)
	{
		BundleInfo* pBundle = project.bundles[bundleIndex];

		if (!bundleName.isEmpty())
		{
			if (pBundle->name != bundleName)
			{
				continue;
			}
		}

		u32 bundleId = hashStringSbdm(pBundle->name);

		files.clear();

		// make abs path for the current bundle folder path
		String absBundlePath = mergePathPath(project.sourceRootPath, pBundle->path);
		String absOutBundlePath = mergePathPath(project.pBuildConfig->outputPath, pBundle->path);

		scanFileSystem(absBundlePath, mask, true, false, files);
		B_LOG_DEBUG("Found %d files in the bundle folder %s", files.size(), pBundle->path.c_str());

		for (size_t i = 0, iCount = files.size(); i < iCount; ++i)
		{
			auto& assetFilename = files[i].fileName;

			if (wildcardCompare(excludeMask.c_str(), assetFilename.c_str(), false))
			{
				continue;
			}

			// get the asset extension
			extractFileExtension(assetFilename.c_str(), ext);
			toLowerCase(ext);

			ResourceType resType = AssetProcessorRegistry::instance().findResourceTypeByFileExtension(ext);

			if (resType == ResourceTypes::None)
			{
				continue;
			}

			// found asset filename
			srcAssetFile = assetFilename;

			// extract the file path
			extractFilePath(srcAssetFile, destFolder);

			// make path to final data
			destFolder.replace(absBundlePath.c_str(), absOutBundlePath.c_str());

			extractFileName(assetFilename, assetFilenameOnly);

			AssetRecord rec;
			DateTime dt;

			fileDateTime(assetFilename, 0, 0, &dt);

			rec.filename = assetFilename;
			rec.destFilename = mergePathFile(destFolder, assetFilenameOnly);
			rec.lastWriteTime = dt;
			rec.resId = hashStringSbdm(assetFilename);
			rec.type = resType;
			rec.userId = project.currentUserId;
			rec.bundleId = bundleId;

			addAsset(rec);
		}
	}
}

void AssetDatabase::deleteAsset(ResourceId resId, bool bDeleteFileAlso)
{
	String str;

	AssetRecord rec = findAsset(resId);

	if (!rec.resId)
	{
		B_LOG_ERROR("Asset not found in the database, id: %u", resId);
		return;
	}

	formatString(str, "DELETE FROM assets WHERE resId=%d", resId);
	sqlite3_exec(m_db, str.c_str(), 0, 0, 0);
	formatString(str, "DELETE FROM dependencies WHERE resId=%d", resId);
	sqlite3_exec(m_db, str.c_str(), 0, 0, 0);

	if (bDeleteFileAlso)
	{
		deleteFile(rec.filename);
	}
}

AssetRecord AssetDatabase::findAsset(ResourceId resId)
{
	AssetRecord rec;
	String str;

	formatString(str, "SELECT * FROM assets WHERE resId=%d", resId);
	sqlite3_stmt* stmt = nullptr;
	const char* tail = nullptr;
	sqlite3_prepare_v2(m_db, str.c_str(), str.length(), &stmt, &tail);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		rec.resId = (ResourceId)sqlite3_column_int(stmt, 0);
		rec.filename = (const char*)sqlite3_column_text(stmt, 1);
		rec.destFilename = (const char*)sqlite3_column_text(stmt, 2);
		rec.lastWriteTime.fromUnixTime(sqlite3_column_int(stmt, 3));
		rec.bHasThumbnail = sqlite3_column_int(stmt, 4);
		rec.bIgnoreCompile = sqlite3_column_int(stmt, 5);
		rec.bDevOnly = sqlite3_column_int(stmt, 6);
		rec.type = (ResourceType)sqlite3_column_int(stmt, 7);
		rec.groupId = sqlite3_column_int(stmt, 8);
		rec.userId = sqlite3_column_int(stmt, 9);
		rec.bundleId = sqlite3_column_int(stmt, 10);

		break;
	}

	return rec;
}

void AssetDatabase::findAssetsForBundle(u32 bundleId, Array<AssetRecord>& outAssets)
{
	AssetRecord rec;
	String str;

	formatString(str, "SELECT * FROM assets WHERE bundleId=%d", bundleId);
	sqlite3_stmt* stmt = nullptr;
	const char* tail = nullptr;
	sqlite3_prepare_v2(m_db, str.c_str(), str.length(), &stmt, &tail);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		rec.resId = (ResourceId)sqlite3_column_int(stmt, 0);
		rec.filename = (const char*)sqlite3_column_text(stmt, 1);
		rec.destFilename = (const char*)sqlite3_column_text(stmt, 2);
		rec.lastWriteTime.fromUnixTime(sqlite3_column_int(stmt, 3));
		rec.bHasThumbnail = sqlite3_column_int(stmt, 4);
		rec.bIgnoreCompile = sqlite3_column_int(stmt, 5);
		rec.bDevOnly = sqlite3_column_int(stmt, 6);
		rec.type = (ResourceType)sqlite3_column_int(stmt, 7);
		rec.groupId = sqlite3_column_int(stmt, 8);
		rec.userId = sqlite3_column_int(stmt, 9);
		rec.bundleId = sqlite3_column_int(stmt, 10);

		outAssets.append(rec);
	}
}

void AssetDatabase::modifyAsset(const AssetRecord& rec)
{
	B_ASSERT_NOT_IMPLEMENTED;
}

void AssetDatabase::createCompileTimeDependency(ResourceId resId, ResourceId dependsOn)
{
	String str;

	formatString(str, "INSERT INTO dependencies (resId, dependsOn, compileTime) VALUES (%d, %d, 1)",
		resId, dependsOn);
	sqlite3_exec(m_db, str.c_str(), 0, 0, 0);
}

void AssetDatabase::createRuntimeDependency(ResourceId resId, ResourceId dependsOn)
{
	String str;

	formatString(str, "INSERT INTO dependencies (resId, dependsOn, compileTime) VALUES (%d, %d, 0)",
		resId, dependsOn);
	sqlite3_exec(m_db, str.c_str(), 0, 0, 0);
}

void AssetDatabase::deleteDependenciesOf(ResourceId resId)
{
	String str;

	formatString(str, "DELETE FROM dependencies WHERE resId = %d", resId);
	sqlite3_exec(m_db, str.c_str(), 0, 0, 0);
}

void AssetDatabase::fillCompileTimeDependenciesOf(ResourceId resId, Array<ResourceId>& deps)
{
	String str;

	formatString(str, "SELECT dependsOn FROM dependencies WHERE resId=%d AND compileTime=1", resId);

	sqlite3_stmt* stmt = nullptr;
	const char* tail = nullptr;
	sqlite3_prepare_v2(m_db, str.c_str(), str.length(), &stmt, &tail);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		ResourceId dependsOn = sqlite3_column_int(stmt, 0);
		deps.append(dependsOn);
	}
}

void AssetDatabase::fillCompileTimeDependantsFor(ResourceId resId, Array<ResourceId>& dependants)
{
	String str;

	formatString(str, "SELECT resId FROM dependencies WHERE dependsOn=%d AND compileTime=1", resId);

	sqlite3_stmt* stmt = nullptr;
	const char* tail = nullptr;
	sqlite3_prepare_v2(m_db, str.c_str(), str.length(), &stmt, &tail);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		ResourceId dependsOn = sqlite3_column_int(stmt, 0);
		dependants.append(dependsOn);
	}
}

void AssetDatabase::checkForModifiedAssets(Array<ResourceId>& outIds, bool bForceAllModified)
{
	String str;

	formatString(str, "SELECT resId, filename, destFilename, lastWriteTime FROM assets WHERE ignoreCompile=0");

	sqlite3_stmt* stmt = nullptr;
	const char* tail = nullptr;
	sqlite3_prepare_v2(m_db, str.c_str(), str.length(), &stmt, &tail);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		ResourceId resId = sqlite3_column_int(stmt, 0);
		const unsigned char* filename = sqlite3_column_text(stmt, 1);
		const unsigned char* destFilename = sqlite3_column_text(stmt, 2);
		int lastWriteTime = sqlite3_column_int(stmt, 3);
		DateTime lwt;

		if (fileDateTime((const char*)filename, 0, 0, &lwt))
		{
			u32 crtTime = lwt.toUnixTime();

			if (crtTime != lastWriteTime || !fileExists((const char*)destFilename) || bForceAllModified)
			{
				markAssetAsModified(resId, crtTime);
				outIds.append(resId);
			}
		}
	}

	B_LOG_INFO("%d assets need to be compiled", outIds.size());
}

void AssetDatabase::addDependentAssetsRecursive(AssetDependencyNode* pNode, Map<ResourceId, AssetDependencyNode*>& allNodes)
{
	Array<ResourceId> dependants;

	fillCompileTimeDependantsFor(pNode->resId, dependants);

	for (auto asset : dependants)
	{
		auto iter = allNodes.find(asset);
		AssetDependencyNode* pDependant;
		
		if (iter == allNodes.end())
		{
			pDependant = new AssetDependencyNode();
			pDependant->resId = asset;
			allNodes.add(asset, pDependant);
		}
		else
		{
			pDependant = iter->value;
		}

		pNode->dependants.append(pDependant);
		addDependentAssetsRecursive(pDependant, allNodes);
	}
}

void AssetDatabase::visitAssetDependencyNode(AssetDependencyNode* pNode, Array<AssetDependencyNode*>& sortedNodes)
{
	if (pNode->bResolvedTemp)
	{
		B_LOG_ERROR("Found a cyclic infinite reference for asset id: %d", pNode->resId);
		return;
	}

	if (!pNode->bResolved)
	{
		pNode->bResolvedTemp = true;

		for (auto node : pNode->dependants)
		{
			B_LOG_DEBUG("Visit dependant: %u", node->resId);
			visitAssetDependencyNode(node, sortedNodes);
		}

		pNode->bResolved = true;
		pNode->bResolvedTemp = false;
		sortedNodes.prepend(pNode);
	}
}

void AssetDatabase::generateSortedAssetNodes(const Array<ResourceId>& changedAssetIds, Array<AssetDependencyNode*>& sortedNodes, Project& project)
{
	Map<ResourceId, AssetDependencyNode*> allNodes;
	Array<ResourceId> assetIds = changedAssetIds;

	// add dependencies
	for (auto assetId : changedAssetIds)
	{
		AssetRecord asset = findAsset(assetId);

		if (!asset.resId)
		{
			B_LOG_ERROR("Cannot find asset with id %u", assetId);
			continue;
		}

		AssetProcessor* pProcessor = AssetProcessorRegistry::instance().findProcessorByResourceType(asset.type);

		if (!pProcessor)
		{
			B_LOG_ERROR("Unknown processor for asset type: %d", asset.type);
			continue;
		}

		B_LOG_DEBUG("Resolving dependencies of : %u:%s", asset.resId, asset.filename.c_str());
		
		auto iter = m_assetJsonDocs.find(asset.resId);
		JsonDocument* pDoc = nullptr;
		
		if (iter == m_assetJsonDocs.end())
		{
			pDoc = new JsonDocument(asset.filename);
			m_assetJsonDocs.add(asset.resId, pDoc);
		}
		else
		{
			pDoc = iter->value;
		}

		pProcessor->resolveDependencies(asset, pDoc, project);
	}

	B_LOG_ERROR("Adding all dependencies...");

	for (auto assetId : changedAssetIds)
	{
		AssetDependencyNode* pNode = new AssetDependencyNode();

		pNode->resId = assetId;
		allNodes.add(assetId, pNode);
		addDependentAssetsRecursive(pNode, allNodes);
	}

	B_LOG_ERROR("Visit and sort assets by dependencies...");

	while (!allNodes.isEmpty())
	{
		for (size_t i = 0; i < allNodes.size(); i++)
		{
			AssetDependencyNode* pNode = allNodes.valueAt(i);

			B_LOG_DEBUG("ResID: %u", pNode->resId);
			
			if (!pNode->bResolved)
			{
				visitAssetDependencyNode(pNode, sortedNodes);
				allNodes.erase(pNode->resId);
				break;
			}
		}

		B_LOG_DEBUG("C: %u", allNodes.size());
	}

	B_LOG_ERROR("Assets to be compiled:");

	for (auto node : sortedNodes)
	{
		B_LOG_DEBUG("Node: %u solved: %d", node->resId, node->bResolved);
	}
}

void AssetDatabase::markAssetAsModified(ResourceId resId, u32 timestamp)
{
	String str;

	formatString(str, "UPDATE assets SET lastWriteTime=%d WHERE resId=%d", timestamp, resId);
	sqlite3_exec(m_db, str.c_str(), 0, 0, 0);
}

void AssetDatabase::markAssetAsCompiled(ResourceId resId, u32 timestamp)
{
	String str;

	formatString(str, "UPDATE assets SET lastWriteTime=%d WHERE resId=%d", timestamp, resId);

	if (SQLITE_OK != sqlite3_exec(m_db, str.c_str(), 0, 0, 0))
	{
		B_ASSERT(!"Cannot mark asset as compiled");
	}
}

bool AssetDatabase::loadDatabase(const String& projectPath)
{
	String dbFilename;

	mergePathFile(projectPath, "assets.db", dbFilename);

	bool bDbExists = fileExists(dbFilename);

	if (SQLITE_OK != sqlite3_open(dbFilename.c_str(), &m_db))
	{
		return false;
	}

	if (!bDbExists)
	{
		createTables();
	}

	return true;
}

void AssetDatabase::createTables()
{
	if (!m_db)
	{
		B_ASSERT(!"No asset database opened");
		return;
	}

	sqlite3_exec(m_db, "CREATE TABLE assets ("
		"resId UNSIGNED INT,"
		"filename CHAR(512),"
		"destFilename CHAR(512),"
		"lastWriteTime INT,"
		"hasThumbnail TINYINT,"
		"ignoreCompile TINYINT,"
		"devOnly TINYINT,"
		"typeId UNSIGNED INT,"
		"groupId UNSIGNED INT,"
		"lastUserId UNSIGNED INT,"
		"bundleId UNSIGNED INT"
		");", 0, 0, 0);

	sqlite3_exec(m_db, "CREATE TABLE groups ("
		"name CHAR(512)"
		");", 0, 0, 0);

	sqlite3_exec(m_db, "CREATE TABLE dependencies ("
		"resId UNSIGNED INT,"
		"dependsOn UNSIGNED INT,"
		"compileTime UNSIGNED INT"
		");", 0, 0, 0);
}

void AssetDatabase::consolidate(bool bDeleteMissingAssets)
{
	String str;

	formatString(str, "SELECT resId, filename, destFilename, lastWriteTime FROM assets");

	sqlite3_stmt* stmt = nullptr;
	const char* tail = nullptr;
	sqlite3_prepare_v2(m_db, str.c_str(), str.length(), &stmt, &tail);

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		ResourceId resId = sqlite3_column_int(stmt, 0);
		const unsigned char* filename = sqlite3_column_text(stmt, 1);
		const unsigned char* destFilename = sqlite3_column_text(stmt, 2);
		int lastWriteTime = sqlite3_column_int(stmt, 3);

		if (bDeleteMissingAssets)
		{
			if (!fileExists((const char*)filename))
			{
				B_LOG_INFO("Deleting asset record and file for id: %u", resId);
				deleteFile((const char*)destFilename);
				deleteAsset(resId);
			}
		}
	}
}

}