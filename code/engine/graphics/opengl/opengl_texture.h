// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "graphics/types.h"
#include "graphics/texture.h"
#include "core/resources/texture_resource.h"

namespace engine
{
class E_API OpenglTexture : public Texture
{
public:
	OpenglTexture();
	~OpenglTexture();
	void setImageData(u32 lod, u32 lodWidth, u32 lodHeight, u32 lodDepth, u8* bitmapData, size_t bitmapDataSize, bool isCompressed) override;
	void setFromResource(ResourceId resId) override;
	void setParameters(const TextureParameters& value) override;
	bool upload() override;

public:
	void freeGL();

	GLuint oglTexId = 0;
	GLuint oglDepthRBO = 0;
	GLuint oglStencilRBO = 0;
	GLuint oglTexType = 0;
	GLenum format = GL_RGBA;
	GLenum internalFormat = GL_RGBA;
	GLenum pixelPrecision = GL_UNSIGNED_BYTE;
	bool autoGenMips = false;
	bool compressed = false;
	u32 mipMapCount = 0;
};

}