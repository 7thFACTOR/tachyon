// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "../common.h"
#include "../asset_processor.h"
#include "base/math/rect.h"

namespace ac
{
using namespace base;

class FontProcessor : public AssetProcessor
{
public:
	FontProcessor();
	~FontProcessor();

	bool import(const String& importFilename, JsonDocument& assetCfg) override;
	bool process(Asset& asset, JsonDocument& assetCfg) override;
};

}