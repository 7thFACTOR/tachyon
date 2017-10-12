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

struct SupportedAssetType
{
	Array<String> assetExtensions;
	ResourceType type = ResourceType::None;
	bool importerOnly = false;
	bool autoImportWhenModified = false;

	SupportedAssetType()
		: type(ResourceType::None)
	{}

	SupportedAssetType(ResourceType resType, const Array<String>& assetExts)
		: assetExtensions(assetExts)
		, type(resType)
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
	const SupportedAssetType& getSupportedAssetType() const { return supportedAssetType; }
	virtual u32 getVersion() const { return 1; }

public:
	Array<String> emittedAssets;

protected:
	SupportedAssetType supportedAssetType;
};

}