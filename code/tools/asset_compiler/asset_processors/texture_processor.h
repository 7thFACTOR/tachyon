// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "../common.h"
#include "../asset_processor.h"
#include "3rdparty/stb_image/stb_image.h"

namespace ac
{
struct TextureImageData
{
	stbi_uc* sourceData = nullptr;
	u64 sourceDataSize = 0;
	u8* compressedData = nullptr;
	u64 compressedDataSize = 0;
};

struct TextureMipMapInfo
{
	u32 width = 0;
	u32 height = 0;
	u32 components = 0;
	Array<TextureImageData> images;
	Array<u64> bitmapDataImageSizes = 0;
	u64 totalBitmapDataSize = 0;
	Array<u64> bitmapDataImageOffsets;
};

class TextureProcessor : public AssetProcessor
{
public:
	TextureProcessor();
	~TextureProcessor();

	bool import(const String& importFilename, JsonDocument& assetCfg) override;
	bool process(Asset& asset, JsonDocument& assetCfg) override;

protected:
	Array<TextureMipMapInfo> mipMaps;

	bool checkNpo2Image(u32 w, u32 h, u32& closestW, u32& closestH);
	void freeData();
};

}