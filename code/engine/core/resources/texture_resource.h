#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/resource.h"
#include "base/math/color.h"
#include "graphics/types.h"

namespace engine
{
enum class TextureFormat
{
	Luminance,
	DXT1,
	DXT3,
	DXT5,
	RGB8,
	RGBA8,
	RGB16F,
	RGBA16F,
	RGB32F,
	RGBA32F,
	
	Count
};

enum class TextureType
{
	Texture1D,
	Texture2D,
	Cubemap,
	Texture1DArray,
	Texture2DArray,
	CubemapArray,
	Volume
};

enum class TextureUsageType
{
	Static,
	Dynamic
};

//! A single mip map level from the texture, it contains 1 or more bitmap images.
//! For example if the texture is a cubemap, it will contain 6 bitmap images, if its a simple texture it will contain 1 image, and if its a volume or texture array, N images
struct TextureMipMap
{
	u32 width = 0; //< mipmap width
	u32 height = 0; //< mipmap height
	Array<u64> bitmapDataImageOffsets; // offsets into bitmapData for each separate bitmap image
	Array<u64> bitmapDataImageSizes; // size of one image into bitmapData for each separate bitmap image
	u64 bitmapDataSize = 0;
	u8* bitmapData = nullptr;
};

struct TextureParameters
{
	TextureFilterType minFilter = TextureFilterType::LinearMipMapLinear;
	TextureFilterType magFilter = TextureFilterType::Linear;
	TextureAddressType texAddressX = TextureAddressType::Repeat;
	TextureAddressType texAddressY = TextureAddressType::Repeat;
	TextureAddressType texAddressZ = TextureAddressType::Repeat;
	f32 minLod = 0;
	f32 maxLod = 1;
	f32 mipMapLodBias = 0;
	f32 maxAnisotropy = 1.0f;
};

struct TextureResource : Resource
{
	TextureParameters parameters;
	Array<TextureMipMap> mipMaps;
	u32 width = 0; //< the actual texture width
	u32 height = 0; //< the actual texture height
	u32 depth = 1; //< number of texture images/slices/array/cube faces, 1 for a normal 2D texture
	TextureType type = TextureType::Texture2D;
	TextureFormat format = TextureFormat::RGB8;
	TextureUsageType usage = TextureUsageType::Static;
	bool isRenderTarget = false;
	bool autoSizeRenderTarget = true;
	bool renderTargetHasDepth = true;
	bool renderTargetHasColor = true;
	bool renderTargetHasStencil = true;
	f32 renderTargetAutoSizeScale = 0.25f;
};

}