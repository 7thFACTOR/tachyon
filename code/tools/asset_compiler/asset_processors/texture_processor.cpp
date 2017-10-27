// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "../common.h"
#include "base/math/vec3.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/file.h"
#include "base/logger.h"
#include "base/json.h"
#include "texture_processor.h"
#include "squish.h"
#include "../asset_compiler.h"
#include "core/resources/texture_resource.h"
#include "../project.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "3rdparty/stb_image/stb_image_resize.h"

namespace ac
{
using namespace base;
using namespace engine;

B_BEGIN_ENUM_STRINGIFY(TextureFormat)
	B_ADD_ENUM_STRINGIFY("dxt1", TextureFormat::DXT1)
	B_ADD_ENUM_STRINGIFY("dxt3", TextureFormat::DXT3)
	B_ADD_ENUM_STRINGIFY("dxt5", TextureFormat::DXT5)
	B_ADD_ENUM_STRINGIFY("rgba8", TextureFormat::RGBA8)
	B_ADD_ENUM_STRINGIFY("rgb8", TextureFormat::RGB8)
	B_ADD_ENUM_STRINGIFY("luminance", TextureFormat::Luminance)
	B_ADD_ENUM_STRINGIFY("rgba16f", TextureFormat::RGBA16F)
	B_ADD_ENUM_STRINGIFY("rgb16f", TextureFormat::RGB16F)
	B_ADD_ENUM_STRINGIFY("rgba32f", TextureFormat::RGBA32F)
	B_ADD_ENUM_STRINGIFY("rgb32f", TextureFormat::RGB32F)
B_END_ENUM_STRINGIFY(TextureFormat)

B_BEGIN_ENUM_STRINGIFY(TextureType)
	B_ADD_ENUM_STRINGIFY("cubemap", TextureType::Cubemap)
	B_ADD_ENUM_STRINGIFY("cubemap_array", TextureType::CubemapArray)
	B_ADD_ENUM_STRINGIFY("1d", TextureType::Texture1D)
	B_ADD_ENUM_STRINGIFY("1d_array", TextureType::Texture1DArray)
	B_ADD_ENUM_STRINGIFY("2d", TextureType::Texture2D)
	B_ADD_ENUM_STRINGIFY("2d_array", TextureType::Texture2DArray)
	B_ADD_ENUM_STRINGIFY("volume", TextureType::Volume)
B_END_ENUM_STRINGIFY(TextureType)

B_BEGIN_ENUM_STRINGIFY(TextureUsageType)
	B_ADD_ENUM_STRINGIFY("static", TextureUsageType::Static)
	B_ADD_ENUM_STRINGIFY("dynamic", TextureUsageType::Dynamic)
B_END_ENUM_STRINGIFY(TextureUsageType)

B_BEGIN_ENUM_STRINGIFY(TextureAddressType)
	B_ADD_ENUM_STRINGIFY("repeat", TextureAddressType::Repeat)
	B_ADD_ENUM_STRINGIFY("clamp", TextureAddressType::Clamp)
B_END_ENUM_STRINGIFY(TextureAddressType)

B_BEGIN_ENUM_STRINGIFY(TextureFilterType)
	B_ADD_ENUM_STRINGIFY("nearestMipMapNearest", TextureFilterType::NearestMipMapNearest)
	B_ADD_ENUM_STRINGIFY("linearMipMapNearest", TextureFilterType::LinearMipMapNearest)
	B_ADD_ENUM_STRINGIFY("nearestMipMapLinear", TextureFilterType::NearestMipMapLinear)
	B_ADD_ENUM_STRINGIFY("linearMipMapLinear", TextureFilterType::LinearMipMapLinear)
	B_ADD_ENUM_STRINGIFY("nearest", TextureFilterType::Nearest)
	B_ADD_ENUM_STRINGIFY("linear", TextureFilterType::Linear)
B_END_ENUM_STRINGIFY(TextureFilterType)

TextureProcessor::TextureProcessor()
{
	//TODO: add more formats, DDS, etc
	Array<String> extensions;

	extensions.append(".png");
	extensions.append(".tga");
	extensions.append(".jpg");
	extensions.append(".bmp");
	extensions.append(".psd");
	supportedAssetInfo = SupportedAssetInfo(ResourceType::Texture, extensions);
}

TextureProcessor::~TextureProcessor()
{}

bool TextureProcessor::checkNpo2Image(u32 w, u32 h, u32& closestW, u32& closestH)
{
	u32 w2 = closestPowerOf2(w);
	u32 h2 = closestPowerOf2(h);

	closestW = w2;
	closestH = h2;

	if (w != w2 || h != h2)
	{
		return false;
	}

	return true;
}

void TextureProcessor::freeData()
{
	// make sure the mipmap image data ptrs in the array are also deleted
	for (size_t i = 0; i < mipMaps.size(); i++)
	{
		for (size_t j = 0; j < mipMaps[i].images.size(); j++)
		{
			delete [] mipMaps[i].images[j].sourceData;
			delete [] mipMaps[i].images[j].compressedData;
		}
	}

	mipMaps.clear();
}

void flipVertically(u32* pixelData, u32 width, u32 height)
{
	for (int x = 0; x < width; x++) 
	{
		for (int y = 0; y < height/2; y++) 
		{
			u32 posxy = y * width + x;
			u32 posxy1 = (height - y - 1) * width + x;
			u32 temp = pixelData[posxy];

			pixelData[posxy]=pixelData[posxy1];
			pixelData[posxy1]=temp;
		}
	}
}

void debugImage(u32* pixelData, u32 width, u32 height)
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height / 2; y++)
		{
			u32 posxy = y * width + x;
			u8* temp = (u8*)&pixelData[posxy];

			temp[0] = 255;
			temp[1] = 0;
			temp[2] = 0;
			temp[3] = 255;
		}
	}
}

bool TextureProcessor::import(const String& importFilename, JsonDocument& assetCfg)
{
	assetCfg.addValue("type", "texture");
	assetCfg.addValue("format", "rgba8");
	assetCfg.addValue("allowNpo2", true);
	assetCfg.addValue("premultiplyAlpha", false);
	assetCfg.addValue("flipVertical", true);
	assetCfg.addValue("textureType", "2d");
	assetCfg.addValue("autoGenMips", true);

	return true;
}

bool TextureProcessor::process(Asset& asset, JsonDocument& assetCfg)
{
	freeData();

	auto cfg = asset.project->currentBuildConfig->processorConfigs["texture"].cfg;
	TextureFormat format = TextureFormat::DXT5;
	bool flipVertical = true; // must flip images, stb_image is giving first pixel top-left
	bool premultiplyAlpha = false;
	bool allowNpo2Texture = false;
	u32 mipMapCount = 1;

	if (cfg)
	{
		String str = cfg->getString("format", B_ENUM_TO_STRING(TextureFormat, (u32)format));
		format = (TextureFormat)B_STRING_TO_ENUM(TextureFormat, str);
		flipVertical = cfg->getBool("flipVertical", flipVertical);
		premultiplyAlpha = cfg->getBool("premultiplyAlpha", premultiplyAlpha);
		allowNpo2Texture = cfg->getBool("allowNpo2", allowNpo2Texture);
	}

	allowNpo2Texture = assetCfg.getBool("allowNpo2", false);
	premultiplyAlpha = assetCfg.getBool("premultiplyAlpha", premultiplyAlpha);
	flipVertical = assetCfg.getBool("flipVertical", flipVertical);
	format = (TextureFormat)B_STRING_TO_ENUM(TextureFormat, assetCfg.getString("format", B_ENUM_TO_STRING(TextureFormat, (u32)format)));
	JsonArray* imagesArray = assetCfg.getArray("images");
	TextureType type = (TextureType)B_STRING_TO_ENUM(TextureType, assetCfg.getString("textureType", "2d"));
	TextureUsageType usage = (TextureUsageType)B_STRING_TO_ENUM(TextureUsageType, assetCfg.getString("usage", "static"));
	bool autoGenMips = assetCfg.getBool("autoGenMips", true);
	u32 wantedComponentCount = 4;
	u32 bytesPerComponent = 1;//TODO: HDR
	int width = 0, height = 0, components = 0;
	bool isRenderTarget = assetCfg.getBool("isRenderTarget", false);
	bool autoSizeRenderTarget = assetCfg.getBool("isRenderTarget", false);
	bool renderTargetHasDepth = assetCfg.getBool("renderTargetHasDepth", true);
	bool renderTargetHasColor = assetCfg.getBool("renderTargetHasColor", true);
	bool renderTargetHasStencil = assetCfg.getBool("renderTargetHasStencil", false);
	f32 renderTargetAutoSizeScale = assetCfg.getF32("renderTargetAutoSizeScale", 1.0f);
	f32 maxAnisotropy = assetCfg.getF32("maxAnisotropy", 1.0f);
	f32 minLod = assetCfg.getF32("minLod", 0);
	f32 maxLod = assetCfg.getF32("maxLod", 1);
	f32 mipMapLodBias = assetCfg.getF32("mipMapLodBias", 0);
	
	TextureFilterType minFilter = (TextureFilterType)B_STRING_TO_ENUM(TextureFilterType, assetCfg.getString("minFilter", "linearMipMapLinear"));
	TextureFilterType magFilter = (TextureFilterType)B_STRING_TO_ENUM(TextureFilterType, assetCfg.getString("magFilter", "linear"));
	TextureAddressType texAddressX = (TextureAddressType)B_STRING_TO_ENUM(TextureAddressType, assetCfg.getString("addressX", "repeat"));
	TextureAddressType texAddressY = (TextureAddressType)B_STRING_TO_ENUM(TextureAddressType, assetCfg.getString("addressY", "repeat"));
	TextureAddressType texAddressZ = (TextureAddressType)B_STRING_TO_ENUM(TextureAddressType, assetCfg.getString("addressZ", "repeat"));

	String imageFilename = asset.absFilename;

	if (imageFilename.isEmpty() && !imagesArray)
	{
		B_LOG_ERROR("No image(s) specified as source for the texture: '" << asset.name << "'");
		return false;
	}

	switch (format)
	{
	case TextureFormat::Luminance:
		wantedComponentCount = 1;
		break;
	case TextureFormat::RGB8:
		wantedComponentCount = 3;
		break;
	case TextureFormat::RGBA8:
		wantedComponentCount = 4;
		break;
	case TextureFormat::RGB16F:
		wantedComponentCount = 3;
		bytesPerComponent = 2;
		break;
	case TextureFormat::RGBA16F:
		wantedComponentCount = 4;
		bytesPerComponent = 2;
		break;
	case TextureFormat::RGB32F:
		wantedComponentCount = 3;
		bytesPerComponent = 4;
		break;
	case TextureFormat::RGBA32F:
		wantedComponentCount = 4;
		bytesPerComponent = 4;
		break;
	case TextureFormat::DXT1:
	case TextureFormat::DXT3:
	case TextureFormat::DXT5:
		B_LOG_INFO("Using DXT compression");
		break;
	}

	//TODO: this needs to be sized to the number of mipmaps which will be generated.
	// For this we need to know the size of the texture and from there compute the number of downsized images from pow^2 (ex: tex size 2048: mip levels computed: 1024, 512, 256, 128, 64, 32, 8, 4, 2, 1 so mipmap count: 12, including original image)

	mipMaps.resize(1);

	if (imageFilename.notEmpty())
	{
		stbi_uc* imgData = stbi_load(imageFilename.c_str(), &width, &height, &components, wantedComponentCount);
		auto mipMapCountW = log2(width);
		auto mipMapCountH = log2(height);

		mipMapCount = B_MIN(mipMapCountW, mipMapCountH);

		B_LOG_INFO("Generating " << mipMapCountW << " mip levels");

		if (!imgData)
		{
			B_LOG_ERROR("Cannot load image: " << imageFilename);
			return false;
		}

		TextureImageData texImgData;
		u32 closestW, closestH;

		if (!checkNpo2Image(width, height, closestW, closestH) && !allowNpo2Texture)
		{
			B_LOG_WARNING("This is a non-power of 2 texture which might not work properly in-game, please resize to: " << closestW << "x" << closestH);

			stbi_image_free(imgData);
			return false;
		}

		if (flipVertical)
		{
			flipVertically((u32*)imgData, width, height);
		}

		texImgData.sourceData = imgData;
		texImgData.sourceDataSize = width * height * wantedComponentCount * bytesPerComponent;
		
		mipMaps[0].width = width;
		mipMaps[0].height = height;
		mipMaps[0].components = components;
		mipMaps[0].images.append(texImgData);
	}
	else if (imagesArray)
	{
		for (size_t i = 0; i < imagesArray->size(); i++)
		{
			auto imageInfo = imagesArray->at(i);
			imageFilename = mergePathName(asset.absFilenamePath, imageInfo->asString());
			stbi_uc* imgData = stbi_load(imageFilename.c_str(), &width, &height, &components, wantedComponentCount);

			if (!imgData)
			{
				B_LOG_ERROR("Cannot load image " << imageInfo->asString());
				return false;
			}

			u32 closestW, closestH;

			if (!checkNpo2Image(width, height, closestW, closestH) && !allowNpo2Texture)
			{
				B_LOG_WARNING("This is a non-power of 2 texture which might not work properly in-game, please resize to: " << closestW << "x" << closestH);
				freeData();
				return false;
			}

			TextureImageData texImgData;

			if (flipVertical)
			{
				flipVertically((u32*)imgData, width, height);
			}

			mipMaps[0].width = width;
			mipMaps[0].height = height;
			mipMaps[0].components = components;

			texImgData.sourceData = imgData;
			texImgData.sourceDataSize = width * height * wantedComponentCount * bytesPerComponent;
			mipMaps[0].images.append(texImgData);
		}
	}

	File file;

	if (!file.open(asset.deployFilename, FileOpenFlags::BinaryWrite))
		return false;

	//TODO: mipmap count, compute mip levels here in the asset compiler, not in the engine with glGenerateMipmaps
	file << mipMapCount;
	// mipmap[0] is the actual original image, so extract info about the texture and write it
	file << mipMaps[0].width;
	file << mipMaps[0].height;
	// mipmap[0] image count is also the depth of the texture, so write depth
	file << (u32)mipMaps[0].images.size();
	file << type;
	file << format;
	file << usage;
	file << isRenderTarget;
	file << autoSizeRenderTarget;
	file << renderTargetHasDepth;
	file << renderTargetHasColor;
	file << renderTargetHasStencil;
	file << renderTargetAutoSizeScale;
	file << minFilter;
	file << magFilter;
	file << texAddressX;
	file << texAddressY;
	file << texAddressZ;
	file << minLod;
	file << maxLod;
	file << mipMapLodBias;
	file << maxAnisotropy;

	size_t currentOffset = 0;

	// compress the images in the mipmap levels
	for (u32 i = 0; i < mipMapCount; i++)
	{
		for (u32 j = 0; j < mipMaps[i].images.size(); j++)
		{
			TextureImageData& imgData = mipMaps[i].images[j];
			
			if (premultiplyAlpha)
			{
				B_LOG_DEBUG("Premultiply alpha...");
				// multiply RGB with A
				u32 len = mipMaps[i].width * mipMaps[i].height;
				u32 stride = 4;
				u8 alpha = 0;

				for (size_t pixel = 0; pixel < len; pixel += 4)
				{
					u8* color = (u8*)&imgData.sourceData[pixel];
					alpha = color[3];
					color[0] *= alpha;
					color[1] *= alpha;
					color[2] *= alpha;
				}
			}

			if (format == TextureFormat::DXT1
				|| format == TextureFormat::DXT3
				|| format == TextureFormat::DXT5)
			{
				B_LOG_INFO("Compressing texture: " << mipMaps[i].width << "x" << mipMaps[i].height << " Format: " << (u32)format);
				// swap RGB to BGR
				u32 len = mipMaps[i].width * mipMaps[i].height;
				u32 stride = 4;
				u8 tmp;
				u8* swapData = (u8*)imgData.sourceData;
				u8 t;

				if (stride != 1)
				{
					for (size_t pixel = 0; pixel < len; ++pixel)
					{
						u8* color = swapData;
						swapData += stride;
						t = color[0];
						color[0] = color[2];
						color[2] = t;
					}
				}

				// create the target data
				int cflags = assetCfg.getBool("highQuality", true) ? squish::kColourMetricUniform : 0;

				if (format == TextureFormat::DXT1)
					cflags |= squish::kDxt1;
				else if (format == TextureFormat::DXT3)
					cflags |= squish::kDxt3;
				else if (format == TextureFormat::DXT5)
					cflags |= squish::kDxt5;

				u32 targetDataSize = squish::GetStorageRequirements(mipMaps[i].width, mipMaps[i].height, cflags);

				imgData.compressedData = new u8[targetDataSize];
				imgData.compressedDataSize = targetDataSize;

				B_LOG_INFO("Needed " << targetDataSize << " for compressed DXT space");
				squish::CompressImage(
					(squish::u8*)imgData.sourceData,
					mipMaps[i].width,
					mipMaps[i].height,
					imgData.compressedData,
					cflags);
			}

			mipMaps[i].bitmapDataImageOffsets.append(currentOffset);

			size_t dataSize = 0;
			u8* data = nullptr;

			if (imgData.compressedData)
			{
				dataSize = imgData.compressedDataSize;
				data = imgData.compressedData;
			}
			else
			{
				dataSize = imgData.sourceDataSize;
				data = imgData.sourceData;
			}

			mipMaps[i].bitmapDataImageSizes.append(imgData.compressedDataSize);
			mipMaps[i].totalBitmapDataSize += dataSize;
			currentOffset += dataSize;
		}
	}

	// write the texture mipmap data
	for (u32 i = 0; i < mipMapCount; i++)
	{
		B_LOG_INFO("Writing texture: " << mipMaps[i].width << "x" << mipMaps[i].height << " Format: " << (u32)format);

		file << mipMaps[i].width;
		file << mipMaps[i].height;
		file << (u32)mipMaps[i].images.size();
		file << mipMaps[i].totalBitmapDataSize;

		// write the offsets to the images
		for (u32 j = 0; j < mipMaps[i].bitmapDataImageOffsets.size(); j++)
		{
			file << mipMaps[i].bitmapDataImageOffsets[j];
			file << mipMaps[i].bitmapDataImageSizes[j];
		}

		// write the actual bitmap data for the images of the mipmap
		for (u32 j = 0; j < mipMaps[i].images.size(); j++)
		{
			TextureImageData& imgData = mipMaps[i].images[j];
			
			if (imgData.compressedData)
			{
				B_LOG_INFO("Writing compressed image data: sizeCompressed: " << imgData.compressedDataSize << " uncompressed: " << imgData.sourceDataSize);
				file.write(imgData.compressedData, imgData.compressedDataSize);
			}
			else
			{
				B_LOG_INFO("Writing uncompressed image data size: " << imgData.sourceDataSize);
				file.write(imgData.sourceData, imgData.sourceDataSize);
			}
		}
	}

	freeData();
	file.close();

	return true;
}

}