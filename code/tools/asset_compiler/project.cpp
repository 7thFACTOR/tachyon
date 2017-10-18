// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "project.h"
#include "base/file.h"
#include "base/util.h"
#include "asset_processor_registry.h"
#include "core/resource.h"

namespace ac
{
B_BEGIN_ENUM_STRINGIFY(BuildPlatformType)
	B_ADD_ENUM_STRINGIFY("win32", BuildPlatformType::Win32)
	B_ADD_ENUM_STRINGIFY("win64", BuildPlatformType::Win64)
	B_ADD_ENUM_STRINGIFY("linux", BuildPlatformType::Linux)
	B_ADD_ENUM_STRINGIFY("osx", BuildPlatformType::OSX)
	B_ADD_ENUM_STRINGIFY("android", BuildPlatformType::Android)
	B_ADD_ENUM_STRINGIFY("ios", BuildPlatformType::iOS)
	B_ADD_ENUM_STRINGIFY("ps4", BuildPlatformType::PS4)
	B_ADD_ENUM_STRINGIFY("xbone", BuildPlatformType::XBone)
	B_ADD_ENUM_STRINGIFY("psvita", BuildPlatformType::PSVita)
B_END_ENUM_STRINGIFY(BuildPlatformType)

B_BEGIN_ENUM_STRINGIFY(BuildType)
	B_ADD_ENUM_STRINGIFY("debug", BuildType::Debug)
	B_ADD_ENUM_STRINGIFY("development", BuildType::Development)
	B_ADD_ENUM_STRINGIFY("shipping", BuildType::Shipping)
B_END_ENUM_STRINGIFY(BuildType)

Project::Project()
	: currentBuildConfig(nullptr)
{}

Project::~Project()
{
	for (auto bundle : bundles)
	{
		delete bundle;
	}
}

bool Project::load(const String& projectFilename)
{
	if (!projectJsonDoc.loadAndParse(projectFilename))
	{
		B_LOG_ERROR("Project JSON has errors: " << projectJsonDoc.getErrorMessage());
		return false;
	}
	
	title = projectJsonDoc.getString("title");
	description = projectJsonDoc.getString("description");
	rootPath = getFilenamePath(projectFilename);
	
	// load builds
	JsonObject* builds = projectJsonDoc.getObject("assetCompiler/buildConfigs");
	String currentBuildConfigName = projectJsonDoc.getString("assetCompiler/defaultBuildConfig");

	if (!builds)
	{
		B_LOG_ERROR("No build configs defined");
		return false;
	}

	currentBuildConfig = nullptr;

	for (size_t i = 0; i < builds->getMembers().size(); i++)
	{
		BuildConfig* buildCfg = new BuildConfig();
		auto configPair = builds->getMembers().at(i);

		if (!configPair.value->isObject())
		{
			B_LOG_ERROR("Build config is not an object");
			return false;
		}

		auto config = configPair.value->asObject();
		String pl = config->getString("platform");

		buildCfg->name = configPair.key;
		buildCfg->platform = (BuildPlatformType)B_STRING_TO_ENUM(BuildPlatformType, config->getString("platform", "win64"));
		buildCfg->type = (BuildType)B_STRING_TO_ENUM(BuildType, config->getString("buildType", "development"));
		buildCfg->outputPath = config->getString("outputPath");
		buildCfg->deployPath = config->getString("deployPath");
		buildCfg->excludeMask = config->getString("excludeMask");

		if (buildCfg->type == BuildType::Shipping && buildCfg->deployPath.isEmpty())
		{
			buildCfg->deployPath = buildCfg->outputPath;
		}

		auto processorConfigsJson = config->getObject("processorConfigs");

		if (processorConfigsJson)
		{
			auto& processorConfigs = processorConfigsJson->getMembers();

			for (size_t j = 0; j < processorConfigs.size(); j++)
			{
				ProcessorConfig procCfg;
				auto procCfgNode = processorConfigs.at(j).value;

				procCfg.cfg = procCfgNode->asObject();
				buildCfg->processorConfigs.add(processorConfigs.at(j).key, procCfg);
			}
		}

		buildConfigs.append(buildCfg);

		if (buildCfg->name == currentBuildConfigName)
		{
			currentBuildConfig = buildCfg;
		}
	}

	// process bundles
	auto bundlesNode = projectJsonDoc.getObject("assetCompiler/bundles");

	if (bundlesNode)
	{
		for (size_t i = 0; i < bundlesNode->getMembers().size(); i++)
		{
			BundleInfo* bundleInfo = new BundleInfo();
			auto bundleNode = bundlesNode->getMembers().at(i).value->asObject();

			bundleInfo->name = bundlesNode->getMembers().at(i).key;
			bundleInfo->path = bundleNode->getString("path");
			bundleInfo->absPath = mergePathPath(rootPath, bundleInfo->path);
			bundleInfo->project = this;

			bundles.append(bundleInfo);
		}
	}

	if (buildConfigs.isEmpty())
	{
		B_LOG_ERROR("No build configs defined for the asset compiler, exiting");
		return false;
	}

	if (bundles.isEmpty())
	{
		BundleInfo* bundleInfo = new BundleInfo();
		
		bundleInfo->name = projectJsonDoc.getString("assetCompiler/defaultBundleName", "gamedata");
		bundleInfo->path = projectJsonDoc.getString("assetCompiler/defaultBundlePath", "");
		bundleInfo->absPath = mergePathPath(rootPath, bundleInfo->path);
		bundleInfo->project = this;
		bundles.append(bundleInfo);

		B_LOG_INFO("No data bundles defined for the asset compiler, using a single bundle: " + bundleInfo->name);
	}

	if (!currentBuildConfig)
	{
		B_LOG_WARNING("No build config name set, using the first one (" << buildConfigs[0]->name << ")");
	}

	return true;
}

void Project::importNewAssets(bool forceImport, bool noCompile, const String& mask, const String& excludeMask)
{
	for (auto& bundle : bundles)
	{
		importAssets(bundle, forceImport, noCompile, mask, excludeMask);
	}
}

void Project::importAssets(BundleInfo* bundle, bool forceImport, bool noCompile, const String& mask, const String& excludeMask)
{
	Array<FoundFileInfo> files;
	String ext;
	String destFolder, filenameMask;
	String assetFilename;
	String outAssetFile;
	String assetName;
	String absBundlePath = mergePathPath(bundle->project->rootPath, bundle->path);
	String fileMask;
	Array<String> supportedExtensions;

	u32 scanTime = getTimeMilliseconds();

	for (auto processor : AssetProcessorRegistry::getInstance().processors)
	{
		for (auto& ext : processor->getSupportedAssetInfo().assetExtensions)
		{
			scanFileSystem(absBundlePath, "*" + ext, files, true);
		}
	}

	B_LOG_SUCCESS("Scanned bundle folders in " << (getTimeMilliseconds() - scanTime)/1000.0f << " sec");

	B_LOG_INFO("Found " << files.size() << " files in the bundle folder: '" << absBundlePath << "'");

	for (size_t i = 0, iCount = files.size(); i < iCount; ++i)
	{
		auto& foundAssetFilename = files[i].filename;
		auto ext = getFilenameExtension(foundAssetFilename);

		if (mask.notEmpty())
		{
			if (!wildcardCompare(mask, foundAssetFilename))
				continue;
		}

		if (excludeMask.notEmpty())
		{
			if (wildcardCompare(excludeMask, foundAssetFilename))
				continue;
		}

		// skip known file types
		if (ext.find(".asset") != String::noIndex
			|| ext.find(".db") != String::noIndex
			|| ext.find(".project") != String::noIndex
			|| ext.find(".cg") != String::noIndex)
			continue;

		importAsset(foundAssetFilename, bundle, forceImport, noCompile);
	}
}

void Project::importAsset(const String& importFilename, BundleInfo* bundle, bool forceImport, bool noCompile)
{
	// get the asset extension
	String ext = getFilenameExtension(importFilename);
	toLowerCase(ext);

	auto processor = AssetProcessorRegistry::getInstance().findProcessorByImportFileExtension(ext);

	if (!processor)
	{
		B_LOG_DEBUG("No asset processor found to import " << importFilename);
		return;
	}

	// extract the file path
	String destFolder = getFilenamePath(importFilename);
	String assetFilename = importFilename;
	// make abs path for the current bundle folder path
	String absBundlePath = mergePathPath(bundle->project->rootPath, bundle->path);
	String absOutBundlePath = mergePathPath(bundle->project->currentBuildConfig->outputPath, bundle->path);

	// make path to final data
	destFolder.replace(absBundlePath, absOutBundlePath);
	assetFilename.replace(beautifyPath(rootPath, true), "");

	bool addToList = true;
	bool doImport = true;
	auto resId = toResourceId(assetFilename);
	auto iterAsset = bundle->assets.find(resId);

	DateTime dt;

	getFileDateTime(importFilename, 0, 0, &dt);

	Asset asset;

	// if asset already in db, don't add it
	if (iterAsset != bundle->assets.end())
	{
		// check for a collision
		if (iterAsset->value.name != assetFilename)
		{
			B_LOG_ERROR("Detected a collision for asset ID: " << resId << "existing asset filename in db: " << iterAsset->value.name << " new discovered filename: " << assetFilename << ". Will use the existing asset, skipping the newly discovered one.");
		}

		addToList = false;
		asset = iterAsset->value;
	}
	else
	{
		asset.project = this;
		asset.name = assetFilename;
		asset.absFilename = importFilename;
		asset.absFilenamePath = getFilenamePath(importFilename);
		asset.resId = toResourceId(asset.name);
		asset.uuid = generateUuid();
		asset.deployFilename = mergePathName(destFolder, toString(asset.resId));
		asset.lastWriteTime = dt.toUnixTime();
		asset.type = processor->getSupportedAssetInfo().outputResourceType;
		asset.bundle = bundle;
		asset.intermediateAsset = processor->getSupportedAssetInfo().importerOnly;

		bundle->assets.add(asset.resId, asset);

		B_LOG_DEBUG("Imported asset " << asset.absFilename << " resType " << (u32)asset.type);
	}

	// if already has an asset file, no need to import
	if (fileExists(importFilename + ".asset") && !forceImport)
	{
		doImport = false;
	}

	// force import anyway of true
	if (forceImport)
		doImport = true;

	JsonDocument assetCfg;

	if (doImport)
	{
		assetCfg.beginDocument();
		assetCfg.addValue("assetFormatVersion", processor->getVersion());
		assetCfg.addValue("uuid", toUuidString(asset.uuid));
		assetCfg.addValue("resourceId", asset.resId);
		processor->import(importFilename, assetCfg);
		assetCfg.endDocument();
		assetCfg.save(importFilename + ".asset");

		// if this processor emitted other assets, then import those too
		for (auto emittedAsset : processor->emittedAssets)
		{
			importAsset(emittedAsset, bundle, forceImport, noCompile);
		}

		processor->emittedAssets.clear();
		B_LOG_DEBUG("Added asset to database: " << assetFilename << " from " << importFilename);

		// process the asset if this asset's processor is not an importer only
		if (!processor->getSupportedAssetInfo().importerOnly
			&& !noCompile)
		{
			processor->process(asset, assetCfg);
		}
	}
	else
	{
		assetCfg.loadAndParse(importFilename + ".asset");
	}
}

bool Project::generateBundle(BundleInfo* bundle)
{
	File file;
	String filename;

	if (currentBuildConfig->type == BuildType::Shipping)
	{
		filename = mergePathName(currentBuildConfig->deployPath, bundle->name + ".bundle");
	}
	else if (currentBuildConfig->type == BuildType::Development)
	{
		filename = mergePathName(currentBuildConfig->outputPath, bundle->name + ".toc");
	}

	if (!bundle->hasModifiedAssets
		&& ((currentBuildConfig->type == BuildType::Shipping) ? fileExists(filename) : true)
		)
	{
		return true;
	}

	B_LOG_INFO("Writing '" << bundle->name << "' bundle TOC...");

	if (currentBuildConfig->type == BuildType::Shipping)
	{
		createPath(currentBuildConfig->deployPath);
	}

	if (!file.open(filename, FileOpenFlags::BinaryWrite))
	{
		B_LOG_ERROR("Cannot open '" << filename << "' TOC/bundle file for write");
		return false;
	}

	auto& bundleAssets = bundle->assets;

	u32 assetCount = 0;
	//TODO: compression of the bundle files
	bool isCompressed = false;
	
	for (auto& asset : bundleAssets)
	{
		if (!asset.value.intermediateAsset)
			assetCount++;
	}
	
	file << isCompressed;
	file.writeUint32(assetCount);

	u64 currentOffset = 0;
	File currentFile;

	Array<u8> fileDataBuffer;
	DateTime dt;
	Array<u64> fileOffsets;
	Array<u64> fileSizes;

	for (auto& item : bundleAssets)
	{
		Asset& asset = item.value;

		if (asset.intermediateAsset)
			continue;

		u64 fileSize = computeFileSize(asset.deployFilename);

		getFileDateTime(asset.deployFilename, nullptr, nullptr, &dt);
		
		file << asset.resId;
		file << asset.type;
		file << currentOffset;
		file << fileSize;
		file << fileSize;//TODO: compress it!
		B_LOG_DEBUG("Writing entry " << asset.name);
		file << asset.name;

		if (currentBuildConfig->type == BuildType::Shipping)
		{
			fileSizes.append(fileSize);
			fileOffsets.append(currentOffset);
		}

		currentOffset += fileSize;
	}

	if (currentBuildConfig->type == BuildType::Shipping)
	{
		int i = 0;
		B_LOG_INFO("Writing big bundle file " << filename  << "...");

		for (auto& item : bundleAssets)
		{
			Asset& asset = item.value;

			if (asset.intermediateAsset)
				continue;

			auto fileSize = fileSizes[i];
			auto fileOffset = fileOffsets[i];
			
			i++;

			fileDataBuffer.resize(fileSize);

			if (currentFile.open(asset.deployFilename, FileOpenFlags::BinaryRead))
			{
				//TODO: compress the data with minizip
				currentFile.read(fileDataBuffer.data(), fileSize);
				file.write(fileDataBuffer.data(), fileSize);
				currentFile.close();
			}
		}
	}

	file.close();

	return true;
}

bool Project::saveDatabase(BundleInfo* bundle)
{
	String dbFilename = mergePathName(rootPath, bundle->name + ".db");
	File file;

	if (file.open(dbFilename, FileOpenFlags::BinaryWrite))
	{
		file << rootPath;
		file << bundle->assets.size();

		for (auto& asset : bundle->assets)
		{
			file << asset.value.resId;
			file << asset.value.uuid;
			file << asset.value.type;
			file << asset.value.name;
			file << asset.value.absFilename;
			file << asset.value.absFilenamePath;
			file << asset.value.deployFilename;
			file << asset.value.lastWriteTime;
			file << asset.value.intermediateAsset;
			file << asset.value.dependencies.size();

			for (auto i = 0; i < asset.value.dependencies.size(); i++)
			{
				file << (u64)asset.value.dependencies[i];
			}
		}
	}

	return true;
}

bool Project::loadDatabase(BundleInfo* bundle)
{
	String dbFilename = mergePathName(rootPath, bundle->name + ".db");
	File file;

	bundle->assets.clear();

	if (file.open(dbFilename, FileOpenFlags::BinaryRead))
	{
		size_t fileCount;
		String projectAbsPath;
		bool refreshAbsFilename = false;

		file >> projectAbsPath;
		file >> fileCount;

		if (projectAbsPath != rootPath)
		{
			B_LOG_INFO("Detected a change of the given project path, refreshing the entry absolute filenames...");
		}
		
		bundle->assets.reserve(fileCount);

		B_LOG_DEBUG("Loading " << fileCount << " database assets");

		for (size_t i = 0; i < fileCount; i++)
		{
			Asset asset;

			asset.project = this;
			file >> asset.resId;
			file >> asset.uuid;
			file >> asset.type;
			file >> asset.name;
			file >> asset.absFilename;
			file >> asset.absFilenamePath;
			file >> asset.deployFilename;
			file >> asset.lastWriteTime;
			file >> asset.intermediateAsset;

			if (refreshAbsFilename)
			{
				asset.absFilename = mergePathName(bundle->absPath, asset.name);
				asset.absFilenamePath = getFilenamePath(asset.absFilename);
			}

			size_t deps = 0;
			
			file >> deps;

			asset.dependencies.resize(deps);
			
			for (auto j = 0; j < deps; j++)
			{
				file >> asset.dependencies[j];
			}

			asset.bundle = bundle;
			
			// check if file exists, if not, then remove the asset by not adding it
			if (fileExists(asset.absFilename))
			{
				bundle->assets.add(asset.resId, asset);
			}

			//B_LOG_DEBUG("\tId: " << asset.resId << "\tFile: " << asset.name << "\tUUID: " << asset.uuid);

			for (auto j = 0; j < deps; j++)
			{
				//B_LOG_DEBUG( "\t\tDependent on: " << asset.dependencies[j]);
			}
		}
	}

	return true;
}

bool Project::loadAllBundleDatabases()
{
	for (auto bundle : bundles)
	{
		if (!loadDatabase(bundle))
		{
			B_LOG_ERROR("Could not load database for bundle: " << bundle->name);
		}
	}

	return true;
}

bool Project::saveAllBundleDatabases()
{
	for (auto bundle : bundles)
	{
		if (!saveDatabase(bundle))
		{
			B_LOG_ERROR("Could not save database for bundle: " << bundle->name);
		}
	}

	return true;
}

BundleInfo* Project::getBundleInfoForFilename(const String& filename)
{
	for (auto bundle : bundles)
	{
		if (filename.find(bundle->absPath) != String::noIndex)
		{
			return bundle;
		}
	}

	return nullptr;
}

void Project::getAllAssets(Array<Asset*>& outAllAssets)
{
	for (auto bundle : bundles)
	{
		for (size_t i = 0; i < bundle->assets.size(); i++)
		{
			outAllAssets.append(&bundle->assets.valueAt(i));
		}
	}

	B_LOG_DEBUG("All assets: " << outAllAssets.size());
}

void Project::checkForModifiedAssets(const Array<Asset*>& allAssets, Array<Asset*>& outModifiedAssets, bool forceModified)
{
	DateTime lwt;

	outModifiedAssets.clear();

	for (auto asset : allAssets)
	{
		if (getFileDateTime(asset->absFilename, 0, 0, &lwt))
		{
			u32 crtTime = lwt.toUnixTime();

			auto proc = AssetProcessorRegistry::getInstance().findProcessorByResourceType(asset->type);
			bool modified = false;

			if (proc)
			{
				modified = proc->isModified(*asset);
			}

			if (crtTime != asset->lastWriteTime
				|| !fileExists(asset->deployFilename)
				|| forceModified
				|| modified)
			{
				outModifiedAssets.append(asset);
			}
		}
	}

	B_LOG_DEBUG("Modified assets: " << outModifiedAssets.size());
}

Asset* Project::findAsset(ResourceId resId)
{
	for (auto bundle : bundles)
	{
		auto iter = bundle->assets.find(resId);

		if (iter != bundle->assets.end())
		{
			return &iter->value;
		}
	}

	return nullptr;
}

Asset* Project::findAsset(AssetUuid uuid)
{
	for (auto bundle : bundles)
	{
		for (auto i = 0; i < bundle->assets.size(); i++)
		{
			if (bundle->assets.valueAt(i).uuid == uuid)
			{
				return &bundle->assets.valueAt(i);
			}
		}
	}

	return nullptr;
}

}