// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "common.h"
#include "base/platform.h"

namespace base
{
	class Stream;
}

namespace ac
{
using namespace base;
struct Project;
struct Asset;

struct SupportedAssetInfo
{
	//! supported asset extensions for importing/processing
	Array<String> assetExtensions;
	//! the resource type generated
	ResourceType outputResourceType = ResourceType::None;
	//! this is just an importer, does not process the data, it can emit one or more assets
	bool importerOnly = false;
	//! when source asset is modified, it will auto re-import and emit assets
	bool autoImportWhenModified = false;

	SupportedAssetInfo()
		: outputResourceType(ResourceType::None)
	{}

	SupportedAssetInfo(ResourceType resType, const Array<String>& assetExts)
		: assetExtensions(assetExts)
		, outputResourceType(resType)
	{}
};

class AssetProcessor
{
public:
	AssetProcessor(){}
	virtual ~AssetProcessor(){}
	virtual bool import(const String& importFilename, JsonDocument& assetCfg) { return false; }
	virtual bool process(Asset& asset, JsonDocument& assetCfg) = 0;
	virtual bool isModified(Asset& asset) { return false; };
	const SupportedAssetInfo& getSupportedAssetInfo() const { return supportedAssetInfo; }
	virtual u32 getVersion() const { return 1; }

public:
	Array<String> emittedAssets;

protected:
	SupportedAssetInfo supportedAssetInfo;
};

}