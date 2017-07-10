#include <string.h>
#include "resources/texture.h"
#include "base/file.h"
#include "base/logger.h"
#include "core/core.h"
#include "core/resource_manager.h"
#include "render/render_manager.h"
#include "base/math/util.h"

namespace engine
{
Texture::Texture()
{
	m_bIsRenderTarget = false;
	m_bKeepImageData = false;
	m_rtWidth = 0;
	m_rtHeight = 0;
	m_bWithColor = true;
	m_bWithDepth = false;
	m_bWithStencil = false;
	m_bUploadPending = false;
	m_renderTargetFormat = eRenderTargetFormat_RGBA8;
	m_bAutoScale = false;
	m_autoScaleFactor = 1.0f;
	m_bAutoGenMipMaps = true;
}

Texture::~Texture()
{
	free();
}

const String& Texture::name() const
{
	return m_name;
}

void Texture::setName(const char* pName)
{
	m_name = pName;
}

Texture::EType Texture::type() const
{
	return m_type;
}

void Texture::setType(EType aType)
{
	m_type = aType;
}

bool Texture::load(Stream* pStream)
{
	Resource::load(pStream);

	//TODO: more mips!
	int crtMip = 0;
	String sig;

	while (!pStream->eof())
	{
		ChunkHeader chunk;

		if (!pStream->readChunkHeader(&chunk))
			break;

		switch (chunk.id)
		{
		case TextureChunkId::Header:
			{
				*pStream >> m_hdr.revision;
				*pStream >> m_hdr.imageCount;
				*pStream >> m_hdr.mipCount;
				*pStream >> m_hdr.format;
				*pStream >> m_hdr.bpp;
				*pStream >> m_hdr.flags;
				*pStream >> m_hdr.compression;
				m_images.resize(m_hdr.mipCount);

				if (m_hdr.revision > TextureFileHeader::kRevision)
				{
					B_LOG_ERROR("Revision in texture is greater than current loader's revision, not loading resource id: %d", m_id);
					return false;
				}

				B_LOG_DEBUG("    Texture has %d images and %d mips/image", m_hdr.imageCount, m_hdr.mipCount);

				break;
			}

		case TextureChunkIds::ImageData:
			{
				for (size_t i = 0; i < m_hdr.mipCount; ++i)
				{
					Array<TextureImageData>& imageMips = m_images[i];

					for (u32 j = 0; j < m_hdr.imageCount; ++j)
					{
						TextureImageData image;

						*pStream >> image.width;
						*pStream >> image.height;
						*pStream >> image.depth;
						*pStream >> image.uncompressedSize;
						*pStream >> image.compressedSize;
					
						B_LOG_DEBUG("	Texture image: %dx%dx%d@%dbpp", image.width, image.height, image.depth, m_hdr.bpp); 
						image.pData = nullptr;

						if (TextureFileHeader::eCompression_None == m_hdr.compression)
						{
							image.pData = new u8[image.uncompressedSize];
							pStream->read(image.pData, image.uncompressedSize);
						}
						else if (TextureFileHeader::eCompression_ZLib == m_hdr.compression)
						{
							image.pData = new u8[image.uncompressedSize];
							//TODO: decompress data with zlib
							//pStream->read(mip.pData, mip.uncompressedSize);
						}

						imageMips.append(image);
					}
				}

				break;
			}

		default:
			pStream->ignoreChunk(&chunk);
		}
	}

	return true;
}

void Texture::freeImageData()
{
	for (size_t i = 0; i < m_images.size(); ++i)
	{
		for (size_t j = 0; j < m_images[i].size(); ++j)
		{
			TextureImageData& image = m_images[i][j];

			if (image.pData)
			{
				delete [] image.pData;
				image.pData = nullptr;
			}
		}
	}
}

void Texture::free()
{
	freeImageData();
	memset(&m_hdr, 0, sizeof(m_hdr));
	m_images.clear();
}

bool Texture::isKeepingImageData() const
{
	return m_bKeepImageData;
}

bool Texture::isRenderTarget() const
{
	return m_bIsRenderTarget;
}

u32 Texture::width(u32 aMip) const
{
	if(m_images.isEmpty())
		return 0;

	if(m_images[aMip].isEmpty())
		return 0;

	return m_images[aMip][0].width;
}

u32 Texture::height(u32 aMip) const
{
	if(m_images.isEmpty())
		return 0;

	if(m_images[aMip].isEmpty())
		return 0;

	return m_images[aMip][0].height;
}

u32 Texture::depth(u32 aMip) const
{
	if(m_images.isEmpty())
		return 0;

	if(m_images[aMip].isEmpty())
		return 0;

	return m_images[aMip][0].depth;
}

Texture::ERenderTargetFormat Texture::renderTargetFormat() const
{
	return m_renderTargetFormat;
}

void Texture::setKeepData(bool bKeep)
{
	m_bKeepImageData = bKeep;
}

void Texture::setAsRenderTarget(
					ERenderTargetFormat aFormat,
					u32 aWidth, u32 aHeight,
					bool bWithColor,
					bool bWithDepth,
					bool bWithStencil)
{
	m_bIsRenderTarget = true;
	m_bWithColor = bWithColor;
	m_bWithDepth = bWithDepth;
	m_bWithStencil = bWithStencil;
	m_rtWidth = aWidth;
	m_rtHeight = aHeight;
	m_renderTargetFormat = aFormat;
}

void Texture::disableRenderTarget()
{
	m_bIsRenderTarget = false;
}

bool Texture::upload()
{
	return false;
}

bool Texture::isUploadPending() const
{
	return m_bUploadPending;
}

void Texture::setRenderTargetAutoScaling(bool bDoScale, f32 aScale)
{
	m_bAutoScale = bDoScale;
	m_autoScaleFactor = aScale;
}

bool Texture::hasRenderTargetAutoScaling() const
{
	return m_bAutoScale;
}

f32 Texture::renderTargetAutoScaleFactor() const
{
	return m_autoScaleFactor;
}

const Array<Array<TextureImageData> >& Texture::images() const
{
	return m_images;
}
}