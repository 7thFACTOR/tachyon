// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/logger.h"
#include "base/assert.h"
#include "core/globals.h"
#include "core/resource_repository.h"
#include "core/resources/texture_resource.h"
#include "opengl_globals.h"
#include "graphics/opengl/opengl_texture.h"
#include "graphics/graphics.h"

namespace engine
{
GLenum cubeFace[6] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

OpenglTexture::OpenglTexture()
{
	oglTexId = 0;
	oglDepthRBO = 0;
	oglStencilRBO = 0;
}

OpenglTexture::~OpenglTexture()
{
	freeGL();
}

void OpenglTexture::freeGL()
{
	if (oglTexId)
	{
		glDeleteTextures(1, &oglTexId);
		CHECK_OPENGL_ERROR;
		oglTexId = 0;
	}

	if (oglDepthRBO)
	{
		glDeleteRenderbuffers(1, &oglDepthRBO);
		oglDepthRBO = 0;
	}

	if (oglStencilRBO)
	{
		glDeleteRenderbuffers(1, &oglStencilRBO);
		oglStencilRBO = 0;
	}
}

void OpenglTexture::setImageData(u32 lod, u32 depthIndex, u32 lodWidth, u32 lodHeight, u32 lodDepth, u8* bitmapData, size_t bitmapDataSize, bool isCompressed)
{
	glBindTexture(oglTexType, oglTexId);
	CHECK_OPENGL_ERROR;
	
	if (renderTarget)
	{
		glTexImage2D(
			oglTexType,
			lod,
			internalFormat,
			width,
			height,
			0,
			format,
			pixelPrecision,
			bitmapData);
		CHECK_OPENGL_ERROR;
	}
	else
	{
		switch (textureType)
		{
		case GL_TEXTURE_1D:
		{
			if (isCompressed)
			{
				glCompressedTexImage1D(
					oglTexType,
					lod,
					internalFormat,
					lodWidth,
					0,
					bitmapDataSize,
					bitmapData);
			}
			else
			{
				glTexImage1D(
					oglTexType,
					lod,
					internalFormat,
					lodWidth,
					0,
					format,
					pixelPrecision,
					bitmapData);
			}
			CHECK_OPENGL_ERROR;
		}
		break;

		case GL_TEXTURE_2D:
		{
			if (isCompressed)
			{
				glCompressedTexImage2D(
					oglTexType,
					lod,
					internalFormat,
					lodWidth,
					lodHeight,
					0,
					bitmapDataSize,
					bitmapData);
			}
			else
			{
				glTexImage2D(
					oglTexType,
					lod,
					internalFormat,
					lodWidth,
					lodHeight,
					0,
					format,
					pixelPrecision,
					bitmapData);
			}
			CHECK_OPENGL_ERROR;
		}
		break;

		case GL_TEXTURE_CUBE_MAP:
		{
			if (isCompressed)
			{
				glCompressedTexImage2D(
					cubeFace[depthIndex],
					lod,
					internalFormat,
					lodWidth,
					lodHeight,
					0,
					bitmapDataSize,
					bitmapData);
			}
			else
			{
				glTexImage2D(
					cubeFace[depthIndex],
					lod,
					internalFormat,
					lodWidth,
					lodHeight,
					0, format, pixelPrecision,
					bitmapData);
			}
			CHECK_OPENGL_ERROR;
		}
		break;

		case GL_TEXTURE_3D:
		{
			if (isCompressed)
			{
				//TODO: this needs to be tested to see if the volume images have the same size
				// DXT images have same bitmap size because they all have same width/height
				glCompressedTexImage3D(
					GL_TEXTURE_3D,
					lod,
					internalFormat,
					lodWidth,
					lodHeight,
					lodDepth,
					0,
					bitmapDataSize,
					bitmapData);
			}
			else
			{
				glTexImage3D(
					GL_TEXTURE_3D,
					lod,
					internalFormat,
					lodWidth,
					lodHeight,
					lodDepth,
					0, format, pixelPrecision,
					bitmapData);
			}
			CHECK_OPENGL_ERROR;
		}
		break;
		}
	}
}

bool OpenglTexture::upload()
{
	ScopedResourceMapping<TextureResource> texRes(textureResourceId);
	B_ASSERT(texRes);

	renderTarget = texRes->isRenderTarget;
	renderTargetAutoSizeScale = texRes->renderTargetAutoSizeScale;
	autoSizeRenderTarget = texRes->autoSizeRenderTarget;
	hasRenderTargetDepth = texRes->renderTargetHasDepth;
	hasRenderTargetColor = texRes->renderTargetHasColor;
	hasRenderTargetStencil = texRes->renderTargetHasStencil;
	width = texRes->width;
	height = texRes->height;
	depth = texRes->depth;
	
	switch (texRes->type)
	{
	case TextureType::Texture1D:
		oglTexType = GL_TEXTURE_1D;
		break;
	case TextureType::Texture2D:
		oglTexType = GL_TEXTURE_2D;
		break;
	case TextureType::Cubemap:
		oglTexType = GL_TEXTURE_CUBE_MAP;
		break;
	case TextureType::Volume:
		oglTexType = GL_TEXTURE_3D;
		break;
	}
	
	if (autoSizeRenderTarget && renderTarget)
	{
		width = (u32)(getGraphics().getViewport().width * renderTargetAutoSizeScale);
		height = (u32)(getGraphics().getViewport().height * renderTargetAutoSizeScale);
	}

	switch (texRes->format)
	{
	case TextureFormat::Luminance:
		format = GL_LUMINANCE;
		break;
	case TextureFormat::DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		internalFormat = GL_SRGB_ALPHA;
		compressed = true;
		break;
	case TextureFormat::DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		internalFormat = GL_SRGB_ALPHA;
		compressed = true;
		break;
	case TextureFormat::DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		internalFormat = GL_SRGB_ALPHA;
		compressed = true;
		break;
	case TextureFormat::RGB8:
		format = GL_RGB;
		internalFormat = GL_RGB;
		break;
	case TextureFormat::RGBA8:
		format = GL_RGBA;
		internalFormat = GL_RGBA;
		break;
	case TextureFormat::RGBA16F:
		format = GL_RGBA16F;
		pixelPrecision = GL_UNSIGNED_SHORT;
		break;
	case TextureFormat::RGBA32F:
		format = GL_RGBA32F;
		pixelPrecision = GL_FLOAT;
		break;
	}

	if (!oglTexId)
	{
		glGenTextures(1, &oglTexId);
		CHECK_OPENGL_ERROR;
	}

	glBindTexture(oglTexType, oglTexId);
	CHECK_OPENGL_ERROR;
	glEnable(oglTexType);
	CHECK_OPENGL_ERROR;
	glTexParameteri(oglTexType, GL_TEXTURE_MAG_FILTER, oglTextureFilterType[(int)parameters.magFilter]);
	CHECK_OPENGL_ERROR;
	glTexParameteri(oglTexType, GL_TEXTURE_MIN_FILTER, oglTextureFilterType[(int)parameters.minFilter]);
	CHECK_OPENGL_ERROR;
	glTexParameteri(oglTexType, GL_TEXTURE_WRAP_T, oglTextureAddressType[(int)parameters.texAddressX]);
	CHECK_OPENGL_ERROR;
	glTexParameteri(oglTexType, GL_TEXTURE_WRAP_S, oglTextureAddressType[(int)parameters.texAddressY]);
	CHECK_OPENGL_ERROR;
	GLfloat fLargest = 0.0f;
	//TODO: maybe cache this value in the graphics?
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	CHECK_OPENGL_ERROR;
	glTexParameteri(oglTexType, GL_TEXTURE_MAX_ANISOTROPY_EXT, parameters.maxAnisotropy * fLargest);
	CHECK_OPENGL_ERROR;

	if (renderTarget)
	{
		glTexImage2D(
			oglTexType,
			0,
			internalFormat,
			width,
			height,
			0,
			format,
			pixelPrecision,
			0);
		CHECK_OPENGL_ERROR;

		if (hasRenderTargetDepth)
		{
			glGenRenderbuffersEXT(1, &oglDepthRBO);
			glBindRenderbufferEXT(GL_RENDERBUFFER, oglDepthRBO);
			glRenderbufferStorageEXT(
				GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
				width, height);
			glBindRenderbufferEXT(GL_RENDERBUFFER, 0);
		}

		if (hasRenderTargetStencil)
		{
			glGenRenderbuffersEXT(1, &oglStencilRBO);
			glBindRenderbufferEXT(GL_RENDERBUFFER, oglStencilRBO);
			glRenderbufferStorageEXT(
				GL_RENDERBUFFER, GL_STENCIL_INDEX8,
				width, height);
			glBindRenderbufferEXT(GL_RENDERBUFFER, 0);
		}
	}
	else
	{
		if (texRes->mipMaps.size())
		{
			width = texRes->mipMaps[0].width;
			height = texRes->mipMaps[0].height;
			depth = texRes->depth;
		}

		switch (texRes->type)
		{
		case TextureType::Texture1D:
			for (size_t mip = 0; mip < texRes->mipMaps.size(); ++mip)
			{
				if (compressed)
				{
					glCompressedTexImage1D(
						oglTexType,
						0,
						internalFormat,
						texRes->mipMaps[mip].width,
						0,
						texRes->mipMaps[mip].bitmapDataSize,
						texRes->mipMaps[mip].bitmapData);
				}
				else
				{
					glTexImage1D(
						oglTexType,
						0,
						internalFormat,
						texRes->mipMaps[mip].width,
						0,
						format,
						pixelPrecision,
						texRes->mipMaps[mip].bitmapData);
				}
				CHECK_OPENGL_ERROR;
			}

			if (autoGenMips)
			{
				glGenerateMipmap(oglTexType);
				CHECK_OPENGL_ERROR;
			}
			break;

		case TextureType::Texture2D:
			for (size_t mip = 0; mip < texRes->mipMaps.size(); ++mip)
			{
				if (compressed)
				{
					glCompressedTexImage2D(
						oglTexType,
						mip,
						internalFormat,
						texRes->mipMaps[mip].width,
						texRes->mipMaps[mip].height,
						0,
						texRes->mipMaps[mip].bitmapDataSize,
						texRes->mipMaps[mip].bitmapData);
				}
				else
				{
					glTexImage2D(
						oglTexType,
						mip,
						internalFormat,
						texRes->mipMaps[mip].width,
						texRes->mipMaps[mip].height,
						0,
						format,
						pixelPrecision,
						texRes->mipMaps[mip].bitmapData);
				}
				CHECK_OPENGL_ERROR;
			}

			if (autoGenMips)
			{
				glGenerateMipmap(oglTexType);
				CHECK_OPENGL_ERROR;
			}

			break;

		case TextureType::Cubemap:
			B_ASSERT(texRes->depth == 6);
			
			if (texRes->depth != 6)
			{
				B_LOG_ERROR("Cubemap texture requires 6 images, but " << texRes->depth << " were found.");
				break;
			}

			for (u32 faceIndex = 0; faceIndex < 6; ++faceIndex)
			{
				for (u32 mip = 0; mip < texRes->mipMaps.size(); ++mip)
				{
					auto& mipmap = texRes->mipMaps[mip];
					auto bmpDataOffset = mipmap.bitmapDataImageOffsets[faceIndex];

					if (compressed)
					{
						glCompressedTexImage2D(
							cubeFace[faceIndex],
							mip,
							internalFormat,
							mipmap.width,
							mipmap.height,
							0,
							mipmap.bitmapDataImageSizes[faceIndex],
							mipmap.bitmapData + bmpDataOffset);
					}
					else
					{
						glTexImage2D(
							cubeFace[faceIndex], mip, internalFormat,
							mipmap.width,
							mipmap.height,
							0, format, pixelPrecision,
							mipmap.bitmapData + bmpDataOffset);
					}
					CHECK_OPENGL_ERROR;
				}
			}

			if (autoGenMips)
			{
				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
				CHECK_OPENGL_ERROR;
			}

			break;

		case TextureType::Volume:
			for (u32 mip = 0; mip < texRes->mipMaps.size(); ++mip)
			{
				if (compressed)
				{
					//TODO: this needs to be tested to see if the volume images have the same size
					// DXT images have same bitmap size because they all have same width/height
					glCompressedTexImage3D(
						GL_TEXTURE_3D, mip, internalFormat,
						texRes->mipMaps[mip].width,
						texRes->mipMaps[mip].height,
						texRes->depth,
						0, 
						texRes->mipMaps[mip].bitmapDataSize,
						texRes->mipMaps[mip].bitmapData);
				}
				else
				{
					glTexImage3D(
						GL_TEXTURE_3D, mip, internalFormat,
						texRes->mipMaps[mip].width,
						texRes->mipMaps[mip].height,
						texRes->depth,
						0, format, pixelPrecision,
						texRes->mipMaps[mip].bitmapData);
				}
				CHECK_OPENGL_ERROR;
			}

			if (autoGenMips)
			{
				glGenerateMipmap(GL_TEXTURE_3D);
				CHECK_OPENGL_ERROR;
			}
			break;
		}
	}

	return true;
}

void OpenglTexture::setFromResource(ResourceId newResourceId)
{
	textureResourceId = newResourceId;
	upload();
}

void OpenglTexture::setParameters(const TextureParameters& value)
{
	parameters = value;
}

}