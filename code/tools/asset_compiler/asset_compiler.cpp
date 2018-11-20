// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "common.h"
#include "asset_compiler.h"
#include "asset_processor.h"
#include "asset_processor_registry.h"
#include "base/platform.h"
#include "base/util.h"
#include "base/math/util.h"
#include "base/logger.h"
#include "base/json.h"
#include "base/file.h"

namespace ac
{
AssetCompiler::AssetCompiler()
{
	SystemInfo si;

	si = getSystemInfo();
	workerThreadCount = si.logicalProcessorCount;
}

AssetCompiler::~AssetCompiler()
{}

bool AssetCompiler::compile(const CommandLineArguments& arguments)
{
	args = arguments;
	String projectFolder = beautifyPath(args.getFreeText());
	String singleFilename = args.getArgValue("file");
	String buildConfigName = args.getArgValue("build-cfg");
	bool forceCompile = args.hasSwitch("force-compile", false) || args.hasSwitch("fc", false);
	bool noCompile = args.hasSwitch("no-compile", false);
	bool forceImport = args.hasSwitch("force-import", false) || args.hasSwitch("fi", false);
	String importFilename = args.getArgValue("import");

	threaded = args.hasSwitch("mt", true);
	workerThreadCount = args.getArgValue("workers", toString((i32)workerThreadCount)).asInt();

	String projectFilename = mergePathName(projectFolder, ".project");

	if (!project.load(projectFilename))
	{
		B_LOG_ERROR("Cannot load project file for " << projectFolder << " (Filename: '" << projectFilename << "')");
		return false;
	}

	if (!buildConfigName.isEmpty())
	{
		for (auto buildCfg : project.buildConfigs)
		{
			if (buildCfg->name == buildConfigName)
			{
				project.currentBuildConfig = buildCfg;
			}
		}
	}

	B_LOG_INFO("Project folder: '" << project.rootPath << "'");
	B_LOG_INFO("Current build config: '" << project.currentBuildConfig->name << "'");

	project.loadAllBundleDatabases();
	
	if (!importFilename.isEmpty())
	{
		B_LOG_INFO("Importing: " << importFilename);
		auto bundleInfo = project.getBundleInfoForFilename(importFilename);
		project.importAsset(importFilename, bundleInfo, forceImport, noCompile);
	}
	else
	{
		project.importNewAssets(forceImport, noCompile);
	}

	Array<ResourceId> changedAssetIds;
	Array<Asset*> allAssets;
	Array<Asset*> modifiedAssets;

	AssetProcessorRegistry::getInstance().debug();

	project.getAllAssets(allAssets);
	project.checkForModifiedAssets(allAssets, modifiedAssets, forceCompile);

	compileAssets(modifiedAssets);

	for (auto bundle : project.bundles)
	{
		project.generateBundle(bundle);
	}

	project.saveAllBundleDatabases();

	B_LOG_SUCCESS("Asset compiler work done.");

	return true;
}

void AssetCompiler::stealWorkFor(AssetCompilerWorker* idleWorker)
{
	AutoLock<Mutex> locker(lockWorkerThreads);
	size_t index = (size_t)randomInteger(workerThreads.size());
	AssetCompilerWorker& worker = workerThreads[index];

	{
		AutoLock<Mutex> lock(worker.lockAssets);

		if (worker.assets.notEmpty())
		{
			Asset* asset = nullptr;

			worker.assets.popFirst(asset);
			idleWorker->addAsset(asset);
		}
	}
}

void AssetCompiler::compileAssets(Array<Asset*>& assets)
{
	// no need for more threads if one single file
	if (assets.size() == 1 || !threaded)
	{
		workerThreadCount = 1;
	}

	workerThreads.resize(workerThreadCount);

	B_LOG_INFO("Creating " << workerThreadCount << " worker threads...");

	for (size_t i = 0; i < workerThreads.size(); ++i)
	{
		B_LOG_INFO("\tStarting worker thread #" << i);
		workerThreads[i].create(nullptr, false, "AssetItemWorker", ThreadPriority::Normal, 0);
		workerThreads[i].initialize(&project, i, args);
		workerThreads[i].compiler = this;
	}

	B_LOG_INFO("\tDistributing initial asset workload");
	u32 threadIndex = 0;

	for (auto asset : assets)
	{
		// assign file to the thread
		workerThreads[threadIndex].addAsset(asset);
		++threadIndex;

		if (threadIndex >= workerThreadCount)
		{
			threadIndex = 0;
		}

		BundleInfo* bundleInfo = asset->bundle;

		if (bundleInfo)
		{
			bundleInfo->hasModifiedAssets = true;
		}
	}

	for (size_t i = 0; i < workerThreads.size(); ++i)
	{
		B_LOG_INFO("Worker " << i << " starting with " << workerThreads[i].getAssetCount() << " assets");
		workerThreads[i].start();
	}

	auto timeElapsed = getTimeMilliseconds();
	
    // wait for the threads to finish
	while (true)
	{
		bool done = true;

		for (size_t i = 0; i < workerThreads.size(); ++i)
		{
			if (workerThreads[i].isBusy())
			{
				done = false;
				break;
			}
		}

		if (done)
		{
			break;
		}

		yield();
	}
	
	timeElapsed = getTimeMilliseconds() - timeElapsed;

	B_LOG_SUCCESS("Workers compiled " << assets.size() << " assets in " << timeElapsed / 1000.0f << " seconds (" << timeElapsed / 1000.0f / 60.0f << " min)");

	// stop workers
	for (auto& worker : workerThreads)
	{
		worker.stop();
	}

	B_LOG_SUCCESS("Done.");
}

}