// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/json.h"
#include "base/util.h"
#include "base/mutex.h"
#include "core/types.h"

namespace ac
{
using namespace base;
using namespace engine;

enum class BuildPlatformType
{
	Win32,
	Win64,
	Linux,
	OSX,
	Android,
	iOS,
	PS4,
	XBone,
	PSVita,

	Count
};

enum class BuildType
{
	Debug,
	Development,
	Shipping
};

typedef u128 AssetUuid;

class Project;

struct ProcessorConfig
{
	JsonObject* cfg = nullptr;
};

struct BuildConfig
{
	String name;
	BuildPlatformType platform = BuildPlatformType::Win64;
	BuildType type = BuildType::Development;
	String outputPath;
	String deployPath;
	Array<String> excludeFileMasks;
	Dictionary<String/*asset processor name*/, ProcessorConfig> processorConfigs;
};

struct Asset
{
	Project* project = nullptr;
	ResourceId resId = nullResourceId;
	AssetUuid uuid = AssetUuid::nullValue();
	String name; // path to asset relative to bundle's root folder, ex: "textures/mecha/test.png"
	String absFilename; // relative to asset compiler exe
	String absFilenamePath; // path from absFilename, relative to asset compiler exe, for convenience
	String relDeployFilename;
	String absDeployFilename;
	ResourceType type = ResourceType::None;
	struct BundleInfo* bundle = nullptr;
	u32 lastWriteTime = 0;
	Array<ResourceId> dependencies;
	bool localizable = true;
	bool intermediateAsset = false;

	Asset()
		: bundle(nullptr)
		, resId(0)
		, lastWriteTime(0)
	{}
};

struct BundleInfo
{
	Project* project;
	String name;
	String path;
	String absPath;
	bool hasModifiedAssets;
	Dictionary<ResourceId, Asset> assets;

	BundleInfo()
		: hasModifiedAssets(false)
		, project(nullptr)
	{}
};

}