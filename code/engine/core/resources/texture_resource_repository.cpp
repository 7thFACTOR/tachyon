// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "core/resources/texture_resource_repository.h"
#include "base/defines.h"
#include "base/assert.h"
#include "base/logger.h"

namespace engine
{
bool TextureResourceRepository::load(Stream& stream, ResourceId resId)
{
	TextureResource& tex = *resources[resId];
	u32 mipMapCount = 0;

	stream >> mipMapCount;
	stream >> tex.width;
	stream >> tex.height;
	stream >> tex.depth;
	stream >> tex.type;
	stream >> tex.format;
	stream >> tex.usage;
	stream >> tex.isRenderTarget;
	stream >> tex.autoSizeRenderTarget;
	stream >> tex.renderTargetHasDepth;
	stream >> tex.renderTargetHasColor;
	stream >> tex.renderTargetHasStencil;
	stream >> tex.renderTargetAutoSizeScale;
	stream >> tex.parameters.minFilter;
	stream >> tex.parameters.magFilter;
	stream >> tex.parameters.texAddressX;
	stream >> tex.parameters.texAddressY;
	stream >> tex.parameters.texAddressZ;
	stream >> tex.parameters.minLod;
	stream >> tex.parameters.maxLod;
	stream >> tex.parameters.mipMapLodBias;
	stream >> tex.parameters.maxAnisotropy;

	B_LOG_DEBUG("\tTexture has "
		<< "Size: " << tex.width << "x" << tex.height << " "
		<< mipMapCount
		<< " mipmaps, type: " << (u32)tex.type
		<< ", format: " << (u32)tex.format
		<< ", usage: " << (u32)tex.usage);

	tex.mipMaps.resize(mipMapCount);

	for (size_t i = 0; i < mipMapCount; ++i)
	{
		TextureMipMap& mipMap = tex.mipMaps[i];
		u32 imageCount = 0;

		stream >> mipMap.width;
		stream >> mipMap.height;
		stream >> imageCount;
		stream >> mipMap.bitmapDataSize;

		B_LOG_DEBUG("\t\tTexture mipmap: " << mipMap.width << "x" << mipMap.height << " depth: " << imageCount);

		// load image offsets and image sizes into bitmap data
		for (u32 j = 0; j < imageCount; j++)
		{
			u64 tmp;

			stream >> tmp;
			mipMap.bitmapDataImageOffsets.append(tmp);
			B_LOG_INFO("\t\t\tBitmap offset  " << tmp);
			stream >> tmp;
			mipMap.bitmapDataImageSizes.append(tmp);
			B_LOG_INFO("\t\t\tBitmap size  " << tmp);
		}

		// allocate mem for all the images in the mipmap
		mipMap.bitmapData = new u8[mipMap.bitmapDataSize];

		// load the image data for this mipmap
		stream.read(mipMap.bitmapData, mipMap.bitmapDataSize);
	}

	return true;
}

void TextureResourceRepository::unload(ResourceId resId)
{
	TextureResource& tex = *resources[resId];

	for (size_t i = 0; i < tex.mipMaps.size(); ++i)
	{
		delete [] tex.mipMaps[i].bitmapData;
	}

	tex.mipMaps.clear();
}

}