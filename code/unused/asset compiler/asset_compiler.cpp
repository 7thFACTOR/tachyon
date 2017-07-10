#include "common.h"
#include "asset_compiler.h"
#include "asset_processor.h"
#include "asset_processor_registry.h"
#include "base/platform.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/json.h"
#include "base/file.h"

namespace ac
{
AssetCompiler::AssetCompiler()
{
	SystemInfo si;

	getSystemInfo(si);
	m_workerThreadCount = si.logicalProcessorCount;
}

AssetCompiler::~AssetCompiler()
{
}

bool AssetCompiler::compile(const ArgsParser& rArgs)
{
	m_args = rArgs;
	String projectFolder = beautifyPath(m_args.nthFreeTextArg());
	String singleFilename = m_args.argValue("file");
	String buildConfigName = m_args.argValue("build-cfg");
	String mask = m_args.argValue("mask", "*.*");
	String excludeMask = m_args.argValue("exclude-mask", "");
	bool bAddAsset = m_args.hasSwitch("add-asset");
	bool bDeleteAsset = m_args.hasSwitch("delete-asset");
	bool bDeleteAssetFile = m_args.hasSwitch("delete-asset-file");
	bool bMarkModifiedAsset = m_args.hasSwitch("mark-modified");
	bool bBulkAddAssets = m_args.hasSwitch("bulk-find-add");
	String bulkAddBundleName = m_args.argValue("bundle", "gamedata");
	bool bConsolidate = m_args.hasSwitch("consolidate");
	bool bForceAllModified = m_args.hasSwitch("force-all-modified");

	m_bThreaded = m_args.hasSwitch("mt", false);
	m_workerThreadCount = m_args.argValue("threads", toString((i32)m_workerThreadCount)).asInt();

	String projectFilename = mergePathFile(projectFolder, "project");

	if (!m_project.load(projectFilename))
	{
		B_LOG_ERROR("Cannot load project file for %s", projectFolder.c_str());
		return false;
	}

	if (!buildConfigName.isEmpty())
	{
		for (size_t i = 0; i < m_project.buildConfigs.size(); i++)
		{
			if (m_project.buildConfigs[i]->name == buildConfigName)
			{
				m_project.setBuildConfig(m_project.buildConfigs()[i]);
			}
		}
	}

	B_LOG_INFO("Project folder: '%s'", m_project.rootPath().c_str()); 
	B_LOG_INFO("Current build config: '%s'", m_project.buildConfig()->name.c_str()); 

	m_project.loadCache(projectFolder);

	if (bAddAsset)
	{
		AssetRecord rec;
		DateTime dt;
		String ext;
		String destFolder;
		String assetFilenameOnly;

		fileDateTime(singleFilename, 0, 0, &rec.lastWriteTime);
		rec.resId = hashStringSbdm(singleFilename);
		rec.filename = singleFilename;
		rec.bundleId = m_project.bundleIdFromAssetFilename(singleFilename);

		// extract the file path
		extractFilePath(singleFilename, destFolder);

		BundleInfo* pBundle = m_project.findBundleById(rec.bundleId);

		B_ASSERT(pBundle);

		if (pBundle)
		{
			// make abs path for the current bundle folder path
			String absBundlePath = mergePathPath(m_project.sourceRootPath, pBundle->path);
			String absOutBundlePath = mergePathPath(m_project.pBuildConfig->outputPath, pBundle->path);

			// make path to final data
			destFolder.replace(absBundlePath.c_str(), absOutBundlePath.c_str());

			extractFileName(singleFilename, assetFilenameOnly);

			rec.destFilename = mergePathFile(destFolder, assetFilenameOnly);
			extractFileExtension(singleFilename, ext);
			rec.type = AssetProcessorRegistry::instance().findResourceTypeByFileExtension(ext);
			rec.userId = m_project.currentUserId;
			m_project.db.addAsset(rec);
			return true;
		}

		return false;
	}

	if (bDeleteAsset)
	{
		m_project.db.deleteAsset(hashStringSbdm(singleFilename), bDeleteAssetFile);
		return true;
	}

	if (bMarkModifiedAsset)
	{
		DateTime dt;
		
		fileDateTime(singleFilename, nullptr, nullptr, &dt);
		m_project.db.markAssetAsModified(hashStringSbdm(singleFilename), dt.toUnixTime());
		return true;
	}

	if (bBulkAddAssets)
	{
		m_project.db.bulkAddAssets(m_project, bulkAddBundleName, mask, excludeMask);
	}

	Array<ResourceId> changedAssetIds;
	Array<AssetDependencyNode*> sortedNodes;

	m_project.db.checkForModifiedAssets(changedAssetIds, bForceAllModified);
	m_project.db.generateSortedAssetNodes(changedAssetIds, sortedNodes, m_project);

	if (!sortedNodes.isEmpty())
	{
		// no need for more threads if one single file
		if (sortedNodes.size() == 1)
		{
			m_workerThreadCount = 1;
		}

		m_workerThreads.resize(m_workerThreadCount);

		B_LOG_INFO("Creating %d worker threads...", m_workerThreadCount);

		for (size_t i = 0; i < m_workerThreads.size(); ++i)
		{
			B_LOG_INFO("\tStarting worker thread #%d...", i);
			m_workerThreads[i].create(nullptr, false, "AssetItemWorker", ThreadPriorities::Normal, 0);
			m_workerThreads[i].setArgs(m_args);
			m_workerThreads[i].initialize(&m_project);
			m_workerThreads[i].setWorkerIndex(i);
		}

		for (auto node : sortedNodes)
		{
			u32 threadIndex = 0;
			u32 minFileItemCount = -1;

			for (size_t j = 0; j < m_workerThreads.size(); ++j)
			{
				u32 count = m_workerThreads[j].assetItemCount();

				if (minFileItemCount > count)
				{
					minFileItemCount = count;
					threadIndex = j;
				}
			}

			// assign file to the thread
			m_workerThreads[threadIndex].addAssetNode(node);
		
			BundleInfo* pBundleInfo = m_project.findBundleById(node->assetRecord.bundleId);

			if (pBundleInfo)
			{
				pBundleInfo->bHasModifiedAssets = true;
			}
		}

		for (size_t i = 0; i < m_workerThreads.size(); ++i)
		{
			m_workerThreads[i].start();
		}

		// wait for the threads
		while (true)
		{
			bool bDone = true;

			for (size_t i = 0; i < m_workerThreads.size(); ++i)
			{
				if (m_workerThreads[i].busy())
				{
					bDone = false;
					break;
				}
			}

			if (bDone)
			{
				break;
			}

			sleep(100);
		}

		// stop workers
		for (size_t i = 0; i < m_workerThreads.size(); ++i)
		{
			m_workerThreads[i].stop();
		}
	}

	for (size_t bundleIndex = 0; bundleIndex < m_project.bundles.size(); bundleIndex++)
	{
		BundleInfo* pBundle = m_project.bundles[bundleIndex];

		m_project.saveToc(pBundle);
	}

	B_LOG_INFO("Done.");

	return true;
}

bool AssetCompiler::loadProject(const String& projectFilename)
{
	if (!m_projectJsonDoc.loadAndParse(projectFilename))
	{
		return false;
	}

	m_project.title = m_projectJsonDoc.getString("title");
	m_project.description = m_projectJsonDoc.getString("description");
	base::extractFilePath(projectFilename, m_project.sourceRootPath);
	// process builds
	JsonObject* builds = m_projectJsonDoc.getObject("assetCompiler/buildConfigs");
	String currentBuildConfig = m_projectJsonDoc.getString("assetCompiler/buildConfig");

	if (!builds)
	{
		B_LOG_ERROR("No build configs defined");
		return false;
	}

	m_project.pBuildConfig = nullptr;

	for (size_t i = 0; i < builds->members().size(); i++)
	{
		BuildConfig* pBuildCfg = new BuildConfig();
		auto configPair = builds->members().at(i);

		if (!configPair.value->isObject())
		{
			B_LOG_ERROR("Build config is not an object");
			return false;
		}

		auto config = configPair.value->asObject();
		String pl = config->getString("platform");

		pBuildCfg->name = configPair.key;
		pBuildCfg->platform = (BuildPlatformType)B_STRING_TO_ENUM(BuildPlatformTypes, config->getString("platform", "win64"));
		pBuildCfg->type = (BuildType)B_STRING_TO_ENUM(BuildTypes, config->getString("buildType", "development"));
		pBuildCfg->outputPath = config->getString("outputPath");
		pBuildCfg->deployPath = config->getString("deployPath");

		if (pBuildCfg->type == BuildTypes::Release && pBuildCfg->deployPath.isEmpty())
		{
			pBuildCfg->deployPath = pBuildCfg->outputPath;
		}
		
		auto processorConfigsJson = config->getObject("processorConfigs");

		if (processorConfigsJson)
		{
			auto& processorConfigs = processorConfigsJson->members();

			for (size_t j = 0; j < processorConfigs.size(); j++)
			{
				ProcessorConfig procCfg;
				auto procCfgNode = processorConfigs.at(j).value;

				procCfg.cfg = procCfgNode->asObject();
				pBuildCfg->processorConfigs.add(processorConfigs.at(j).key, procCfg);
			}
		}

		m_project.buildConfigs.append(pBuildCfg);

		if (pBuildCfg->name == currentBuildConfig)
		{
			m_project.pBuildConfig = pBuildCfg;
		}
	}

	// process bundles
	auto bundlesNode = m_projectJsonDoc.getObject("assetCompiler/bundles");

	if (bundlesNode)
	{
		for (size_t i = 0; i < bundlesNode->members().size(); i++)
		{
			BundleInfo* bundleInfo = new BundleInfo();
			auto bundleNode = bundlesNode->members().at(i).value->asObject();
		
			bundleInfo->name = bundlesNode->members().at(i).key;
			bundleInfo->path = bundleNode->getString("path");
			bundleInfo->pProject = &m_project;

			m_project.bundles.append(bundleInfo);
		}
	}

	if (m_project.buildConfigs.isEmpty())
	{
		B_LOG_ERROR("No build configs defined for the asset compiler, exiting");
		return false;
	}

	if (m_project.bundles.isEmpty())
	{
		BundleInfo* bundleInfo = new BundleInfo();
		bundleInfo->name = m_projectJsonDoc.getString("assetCompiler/defaultBundleName", "gamedata");
		bundleInfo->path = m_projectJsonDoc.getString("assetCompiler/defaultBundlePath", "");
		bundleInfo->pProject = &m_project;
		m_project.bundles.append(bundleInfo);

		B_LOG_INFO("No data bundles defined for the asset compiler, using a single bundle: %s", bundleInfo->name.c_str());
	}

	if (!m_project.pBuildConfig)
	{
		B_LOG_WARNING("No build config name set, using the first one (%s)", m_project.buildConfigs[0]->name.c_str());
	}

	return true;
}

}