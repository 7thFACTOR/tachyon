#pragma once
#include "base/array.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/resource.h"
#include "base/math/color.h"

namespace engine
{
namespace TextureChunkIds
{
	enum TextureChunkId
	{
		Header,
		ImageData
	};
}
typedef TextureChunkIds::TextureChunkId TextureChunkId;

struct TextureFileHeader
{
	enum EFormat
	{
		eFormat_RGBA,
		eFormat_BGRA,
		eFormat_Luminance,
		eFormat_DXT1,
		eFormat_DXT3,
		eFormat_DXT5
	};

	enum ECompression
	{
		eCompression_None,
		eCompression_ZLib
	};

	enum EFlags
	{
		eFlag_CubeMap = B_BIT(0),
		eFlag_VolumeMap = B_BIT(1)
	};

	static const u32 kRevision = 1;
	u16 revision;
	u16 imageCount;
	u16 mipCount;
	u8 format;
	u8 bpp;
	u32 flags;
	u8 compression;
};

struct TextureImageData
{
	u16 width;
	u16 height;
	u16 depth;
	u32 uncompressedSize;
	u32 compressedSize;
	u8* pData;
};

struct E_API TextureAtlasRect
{
	f32 u, v, uWidth, vHeight;
};

struct E_API TextureAtlasRects
{
	Array<TextureAtlasRect> m_rects;
};

//! A texture resource class
//! @remark Texture objects are created through the GraphicsProvider newInstanceOf() method, and NOT directly,
//! because they are specific to the graphics API currently used
class E_API Texture : public Resource
{
public:
	//! The texture type
	enum EType
	{
		eType_1D,
		eType_2D,
		eType_Cubemap,
		eType_1DArray,
		eType_2DArray,
		eType_CubemapArray,
		eType_Volume
	};

	//! The texture usage mode
	enum EUsage
	{
		eUsage_Static,
		eUsage_Dynamic
	};

	//! The render target texture format
	enum ERenderTargetFormat
	{
		eRenderTargetFormat_RGB8,
		eRenderTargetFormat_RGB16F,
		eRenderTargetFormat_RGB32F,
		eRenderTargetFormat_RGBA8,
		eRenderTargetFormat_RGBA16F,
		eRenderTargetFormat_RGBA32F
	};

protected:
	//! you cannot create the texture directly, only like this:\n
	//! #1 Texture* pTexture = resources().load<Texture>( "textures/some" );\n
	//! #2 Texture* pTexture = gfx().newTexture();\n
	Texture();

public:
	virtual ~Texture();

	const String& name() const;
	void setName(const char* pName);
	EType type() const;
	void setType(EType aType);
	virtual bool load(Stream* pStream);
	//! is this texture keeping its image data after the upload to the video card ?\n
	//! else the image data will be discarded
	bool isKeepingImageData() const;
	//! is this texture a render target ?
	bool isRenderTarget() const;
	//! \return the texture's width at a mip level
	virtual u32 width(u32 aMip = 0) const;
	//! \return the texture's height
	virtual u32 height(u32 aMip = 0) const;
	//! \return the texture's depth
	virtual u32 depth(u32 aMip = 0) const;
	//! \return the render target format
	ERenderTargetFormat renderTargetFormat() const;
	//! set keep image data in RAM, even after uploading texture to video card
	void setKeepData(bool bKeep);
	//! set this texture to be a render target
	virtual void setAsRenderTarget(
					ERenderTargetFormat aFormat,
					u32 aWidth, u32 aHeight,
					bool bWithColor, bool bWithDepth, bool bWithStencil);
	//! disable as a render target
	virtual void disableRenderTarget();
	//! upload the texture to video card
	//! \return false if a previous upload is pending, data will not be upload
	virtual bool upload();
	virtual void freeImageData();
	virtual void free();
	//! \return true if data is pending for upload
	bool isUploadPending() const;
	//! set the scaling of the render target, dependent on resolution (0..1 f32)
	//! when the resolution changes, the render target texture will be automatically scaled
	void setRenderTargetAutoScaling(bool bDoScale = true, f32 aScale = 1.0f);
	//! get auto scale dependent on resolution
	//! \return true if auto scaling is enabled for this texture
	bool hasRenderTargetAutoScaling() const;
	f32 renderTargetAutoScaleFactor() const;
	const Array<Array<TextureImageData> >& images() const;

protected:
	EType m_type;
	String m_name;
	bool m_bAutoScale;
	bool m_bAutoGenMipMaps;
	f32 m_autoScaleFactor;
	u32 m_rtWidth, m_rtHeight;
	bool m_bKeepImageData, m_bIsRenderTarget;
	bool m_bWithColor, m_bWithDepth, m_bWithStencil;
	ERenderTargetFormat m_renderTargetFormat;
	bool m_bUploadPending;
	TextureFileHeader m_hdr;
	Array<Array<TextureImageData> > m_images;
};
}