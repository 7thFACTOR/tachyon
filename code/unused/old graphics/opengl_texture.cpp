#include "opengl_globals.h"
#include "base/logger.h"
#include "core/resource_repository.h"
#include "core/globals.h"
#include "opengl_texture.h"
#include "render/opengl/opengl_graphics.h"

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

OpenGLTexture::OpenGLTexture()
{
	oglTexId = 0;
	oglDepthRBO = 0;
	oglStencilRBO = 0;
}

OpenGLTexture::~OpenGLTexture()
{}

void OpenGLTexture::free()
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

void OpenGLTexture::setData(void* data, size_t dataSize)
{
	//TODO
	B_ASSERT_NOT_IMPLEMENTED;
}

void OpenGLTexture::resize(u32 width, u32 height, u32 depth)
{
	//TODO
	B_ASSERT_NOT_IMPLEMENTED;
}

bool OpenGLTexture::upload()
{
	ScopedResourceMapping<TextureResource> texResource(texture);
	B_ASSERT(texResource);
	GLenum format = GL_RGBA;
	GLenum internalFormat = GL_RGBA;
	GLenum pixelPrecision = GL_UNSIGNED_BYTE;
	bool autoGenMips = texResource->mipCount == 1;
	bool compressed = false;

	renderTarget = texResource->renderTarget;
	renderTargetAutoSizeScale = texResource->autoSizeScale;
	autoSizeRenderTarget = texResource->autoSizeRenderTarget;
	hasRenderTargetDepth = texResource->renderTargetDepth;
	hasRenderTargetColor = texResource->renderTargetColor;
	hasRenderTargetStencil = texResource->renderTargetStencil;

	switch (texResource->type)
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
		width = (u32)(oglGraphics->getViewport().width * renderTargetAutoSizeScale);
		height = (u32)(oglGraphics->getViewport().height * renderTargetAutoSizeScale);
	}

	switch (texResource->format)
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
		if (texResource->imageCount && texResource->mipCount)
		{
			width = texResource->images[0].mips[0].width;
			height = texResource->images[0].mips[0].height;
			depth = texResource->images[0].mips[0].depth;
		}

		switch (texResource->type)
		{
		case TextureType::Texture1D:
			for (size_t mip = 0; mip < texResource->mipCount; ++mip)
			{
				if (compressed)
				{
					glCompressedTexImage1D(
						oglTexType,
						0,
						internalFormat,
						texResource->images[0].mips[mip].width,
						0,
						texResource->images[0].mips[mip].compressedSize,
						texResource->images[0].mips[mip].data);
				}
				else
				{
					glTexImage1D(
						oglTexType,
						0,
						internalFormat,
						texResource->images[0].mips[mip].width,
						0,
						format,
						pixelPrecision,
						texResource->images[0].mips[mip].data);
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
			for (size_t mip = 0; mip < texResource->mipCount; ++mip)
			{
				if (compressed)
				{
					glCompressedTexImage2D(
						oglTexType,
						mip,
						internalFormat,
						texResource->images[0].mips[mip].width,
						texResource->images[0].mips[mip].height,
						0,
						texResource->images[0].mips[mip].compressedSize,
						texResource->images[0].mips[mip].data);
				}
				else
				{
					glTexImage2D(
						oglTexType,
						mip,
						internalFormat,
						texResource->images[0].mips[mip].width,
						texResource->images[0].mips[mip].height,
						0,
						format,
						pixelPrecision,
						texResource->images[0].mips[mip].data);
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
			B_ASSERT(texResource->imageCount == 6);
			
			if (texResource->imageCount != 6)
			{
				B_LOG_ERROR("Cubemap texture doesnt have 6 images");
				break;
			}

			for (u32 i = 0; i < 6; ++i)
			{
				for (u32 mip = 0; mip < texResource->mipCount; ++mip)
				{
					if (compressed)
					{
						glCompressedTexImage2D(
							cubeFace[i],
							mip,
							internalFormat,
							texResource->images[i].mips[mip].width,
							texResource->images[i].mips[mip].height,
							0,
							texResource->images[i].mips[mip].compressedSize,
							texResource->images[i].mips[mip].data);
					}
					else
					{
						glTexImage2D(
							cubeFace[i], mip, internalFormat,
							texResource->images[i].mips[mip].width,
							texResource->images[i].mips[mip].height,
							0, format, pixelPrecision,
							texResource->images[i].mips[mip].data);
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
			for (u32 mip = 0; mip < texResource->mipCount; ++mip)
			{
				if (compressed)
				{
					glCompressedTexImage3D(
						GL_TEXTURE_3D, mip, internalFormat,
						texResource->images[0].mips[mip].width,
						texResource->images[0].mips[mip].height,
						texResource->images[0].mips[mip].depth,
						0, 
						texResource->images[0].mips[mip].compressedSize,
						texResource->images[0].mips[mip].data);
				}
				else
				{
					glTexImage3D(
						GL_TEXTURE_3D, mip, internalFormat,
						texResource->images[0].mips[mip].width,
						texResource->images[0].mips[mip].height,
						texResource->images[0].mips[mip].depth,
						0, format, pixelPrecision, texResource->images[0].mips[mip].data);
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

void OpenGLTexture::setResource(ResourceId resId)
{
	texture = resId;
	upload();
}

void OpenGLTexture::setParameters(const TextureParameters& value)
{
	parameters = value;
}

const TextureParameters& OpenGLTexture::getParameters() const
{
	return parameters;
}

}