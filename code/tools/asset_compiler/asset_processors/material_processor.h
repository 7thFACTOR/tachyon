// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "../common.h"
#include "../asset_processor.h"
#include "base/json.h"

namespace ac
{
class MaterialProcessor : public AssetProcessor
{
public:
	MaterialProcessor();
	~MaterialProcessor();

	bool import(const String& importFilename, JsonDocument& assetCfg) override;
	bool process(Asset& asset, JsonDocument& assetCfg) override;
};

}