#include "../common.h"
#include "base/math/vec3.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/file.h"
#include "base/logger.h"
#include "base/json.h"
#include "texture_atlas_processor.h"
#include "texture_processor.h"
#include "squish.h"
#include "../asset_compiler.h"
#include "core/resources/texture_resource.h"
#include "3rdparty/stb_image/stb_image_write.h"
#include "../project.h"
#include "core/resources/texture_atlas_resource.h"
#include "base/misc/image_atlas.h"

namespace ac
{
using namespace base;
using namespace engine;

B_BEGIN_ENUM_STRINGIFY(ImageAtlasPackPolicy)
	B_ADD_ENUM_STRINGIFY("guillotine", ImageAtlasPackPolicy::Guillotine)
	B_ADD_ENUM_STRINGIFY("maxrects", ImageAtlasPackPolicy::MaxRects)
	B_ADD_ENUM_STRINGIFY("shelfbin", ImageAtlasPackPolicy::ShelfBin)
	B_ADD_ENUM_STRINGIFY("skyline", ImageAtlasPackPolicy::Skyline)
B_END_ENUM_STRINGIFY(ImageAtlasPackPolicy)

TextureAtlasProcessor::TextureAtlasProcessor()
{
	Array<String> extensions;

	extensions.append(".atlas");
	supportedAssetType = SupportedAssetType(ResourceType::TextureAtlas, extensions);
}

TextureAtlasProcessor::~TextureAtlasProcessor()
{}

bool TextureAtlasProcessor::import(const String& importFilename, JsonDocument& assetCfg)
{
	assetCfg.addValue("type", "atlas");

	return true;
}

bool TextureAtlasProcessor::process(Asset& asset, JsonDocument& assetCfg)
{
	auto cfg = asset.project->currentBuildConfig->processorConfigs["texture_atlas"].cfg;
	JsonDocument atlasAsset(asset.absFilename);

	u32 atlasWidth = atlasAsset.getI32("atlasWidth", 1024);
	u32 atlasHeight = atlasAsset.getI32("atlasHeight", 1024);

	String packPolicy = atlasAsset.getString("packPolicy", "guillotine");
	bool useWasteMap = atlasAsset.getBool("useWasteMap", false);
	u32 border = atlasAsset.getI32("border", 1);
	u32 border2 = border * 2;
	JsonArray* imagesArray = atlasAsset.getArray("images");
	String assetFilename;

	ImageAtlas atlas(atlasWidth, atlasHeight);

	assetFilename = getFilenameName(asset.absFilename);

	if (!imagesArray)
	{
		B_LOG_ERROR("No image(s) specified as source for the atlas: '" << asset.name << "'");
		return false;
	}

	if (imagesArray)
	{
		int imgWidth, imgHeight;
		int components = 0, wantedComponents = 4;

		for (size_t i = 0; i < imagesArray->size(); i++)
		{
			auto imageInfo = imagesArray->at(i)->asObject();
			String filename;
			filename = mergePathName(asset.absFilenamePath, imageInfo->getString("image"));
			stbi_uc* imgData = stbi_load(filename.c_str(), &imgWidth, &imgHeight, &components, wantedComponents);

			if (!imgData)
			{
				B_LOG_ERROR("Cannot load image " << filename);
				return false;
			}

			atlas.addItem(imageInfo->getString("name"), (u32*)imgData, imgWidth, imgHeight);
		}
	}


	atlas.pack();

	String pngFilename;
	String texFilename;

	pngFilename = changeFilenameExtension(asset.absFilename, ".png");
	stbi_write_png(pngFilename.c_str(), atlasWidth, atlasHeight, 4, atlas.getAtlasImageBuffer(), atlasWidth * 4);

	texFilename = changeFilenameExtension(asset.absFilename, ".png");

	File atlasFile;

	if (!atlasFile.open(asset.deployFilename, FileOpenFlags::BinaryWrite))
	{
		return false;
	}

	atlasFile << (u64)toResourceId(texFilename);
	atlasFile << (u32)atlas.getItems().size();

	asset.dependencies.appendUnique(toResourceId(texFilename));

	for (auto& item : atlas.getItems())
	{
		atlasFile << item.value->name;
		atlasFile << item.value->wasRotated;
		atlasFile << (f32)item.value->packedRect.x / atlasWidth;
		atlasFile << (f32)item.value->packedRect.y / atlasHeight;
		atlasFile << (f32)item.value->packedRect.width / atlasWidth;
		atlasFile << (f32)item.value->packedRect.height / atlasHeight;
	}

	atlasFile.close();

	return true;
}

bool TextureAtlasProcessor::isModified(Asset& asset)
{
	String texFilename = changeFilenameExtension(asset.absFilename, ".png");

	if (!fileExists(texFilename))
		return true;

	return false;
}

}