// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "graphics/types.h"
#include "core/resources/texture_resource.h"

namespace engine
{
class E_API Texture
{
public:
	Texture() {}
	virtual ~Texture() {}
	virtual void setImageData(u32 lod, u32 lodWidth, u32 lodHeight, u32 lodDepth, u8* bitmapData, size_t bitmapDataSize, bool isCompressed) = 0;
	virtual void setFromResource(ResourceId resId) = 0;
	virtual void setParameters(const TextureParameters& value) = 0;
	virtual bool upload() = 0;
	inline const TextureParameters& getParameters() const { return parameters; }
	inline u32 getWidth() const { return width; }
	inline u32 getHeight() const { return height; }
	inline u32 getDepth() const { return depth; }

protected:
	ResourceId textureResourceId = nullResourceId;
	TextureParameters parameters;
	bool autoSizeRenderTarget = true;
	f32 renderTargetAutoSizeScale = 1;
	bool renderTarget = false;
	bool hasRenderTargetDepth = true;
	bool hasRenderTargetColor = true;
	bool hasRenderTargetStencil = true;
	u32 width = 0;
	u32 height = 0;
	u32 depth = 1;
};

}