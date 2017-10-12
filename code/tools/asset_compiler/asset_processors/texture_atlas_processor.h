// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "../common.h"
#include "../asset_processor.h"
#include "3rdparty/stb_image/stb_image.h"
#include "base/math/rect.h"

namespace ac
{
using namespace base;

class TextureAtlasProcessor : public AssetProcessor
{
public:
	TextureAtlasProcessor();
	~TextureAtlasProcessor();

	bool import(const String& importFilename, JsonDocument& assetCfg) override;
	bool process(Asset& asset, JsonDocument& assetCfg) override;
	bool isModified(Asset& asset) override;
};

}