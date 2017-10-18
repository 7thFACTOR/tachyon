// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "../common.h"
#include "base/math/vec3.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/file.h"
#include "base/logger.h"
#include "base/json.h"
#include "font_processor.h"
#include "../asset_compiler.h"
#include "../project.h"
#include "core/resources/texture_atlas_resource.h"
#include "core/resources/font_resource.h"
#include "base/misc/image_atlas.h"

namespace ac
{
using namespace base;
using namespace engine;

FontProcessor::FontProcessor()
{
	Array<String> extensions;

	extensions.append(".ttf");
	extensions.append(".otf");
	supportedAssetInfo = SupportedAssetInfo(ResourceType::Font, extensions);
}

FontProcessor::~FontProcessor()
{}

bool FontProcessor::import(const String& importFilename, JsonDocument& assetCfg)
{
	assetCfg.addValue("type", "font");
	assetCfg.addValue("dynamic", false);

	return true;
}

bool FontProcessor::process(Asset& asset, JsonDocument& assetCfg)
{
	auto cfg = asset.project->currentBuildConfig->processorConfigs["font"].cfg;

	if (cfg)
	{}

	String fontFilename = assetCfg.getString("fontFile");
	bool dynamic = assetCfg.getBool("dynamic");
	String assetFilename;

	assetFilename = getFilenameName(asset.absFilename);

	File fontResFile;

	if (!fontResFile.open(asset.deployFilename, FileOpenFlags::BinaryWrite))
	{
		return false;
	}

	if (dynamic)
	{
		Array<u8> data;
		File fntFile(fontFilename, FileOpenFlags::BinaryRead);

		if (!fntFile.isEndOfStream())
		{
			data.resize(fntFile.getSize());
			fntFile.read(data.data(), data.size());
		}

		fontResFile << (u64)data.size();

		if (!data.isEmpty())
		{
			fontResFile.write(data.data(), data.size());
		}
	}

	//TODO:
	//fontResFile << toResourceId(assetFilename);
	//fontResFile << (u32)atlas.getItems().size();

	//for (auto& item : atlas.getItems())
	//{
	//	atlasFile << item.value->name;
	//	atlasFile << item.value->wasRotated;
	//	atlasFile << (f32)item.value->packedRect.x / atlasWidth;
	//	atlasFile << (f32)item.value->packedRect.y / atlasHeight;
	//	atlasFile << (f32)item.value->packedRect.width / atlasWidth;
	//	atlasFile << (f32)item.value->packedRect.height / atlasHeight;
	//}

	fontResFile.close();

	return true;
}

}

