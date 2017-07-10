#include "gui/font.h"
#include "core/resource_repository.h"
#include "core/globals.h"
#include "render/resources/font_resource.h"
#include "base/memory_stream.h"
#include "render/texture.h"
#include "render/graphics.h"
#include "render/resources/texture_resource.h"

namespace engine
{
Font::Font()
{}

Font::Font(ResourceId font, u32 faceSize, u32 atlasSize)
{
	setFont(font, faceSize, atlasSize);
}

void Font::setFont(ResourceId font, u32 faceSize, u32 atlasSize)
{
	cachedFont.free();
	this->font = font;

	if (getResources().loadNow(font))
	{
		ScopedResourceMapping<FontResource> fntRes(font);

		if (fntRes)
		{
			MemoryStream memStream;

			memStream.openForRead(fntRes->fontFileData, fntRes->fontFileDataSize);
			cachedFont.load(memStream, faceSize, atlasSize);
			cachedFont.precacheLatinAlphabetGlyphs();
		}
	}
}

void Font::setSize(u32 faceSize, u32 atlasSize)
{
	setFont(font, faceSize, atlasSize);
}

RectF Font::getGlyphTexCoords(u32 code)
{
	FontGlyph* glyph = cachedFont.getGlyph(code);

	if (!glyph)
	{
		return RectF();
	}

	return glyph->texRect;
}

void Font::updateTexture()
{
	if (texture == invalidResourceId)
	{
		texture = getResources().createDynamicResource(ResourceType::Texture);
		B_ASSERT(texture != invalidResourceId);
	}

	{
		ScopedResourceMapping<TextureResource> texRes(texture);

		texRes->format = TextureFormat::RGBA8;
		texRes->imageCount = 1;

		if (!texRes->images)
		{
			texRes->images = new TextureImage[1];
			texRes->images[0].mips = new TextureMip[1];
			texRes->images[0].mips[0].data = (u8*)cachedFont.getAtlas().getAtlasImageBuffer();
			texRes->images[0].mips[0].dataSize = cachedFont.getAtlas().getAtlasImageBufferSize();
			texRes->images[0].mips[0].compressedSize = texRes->images[0].mips[0].dataSize;
			texRes->images[0].mips[0].depth = 1;
			texRes->images[0].mips[0].width = cachedFont.getAtlas().getWidth();
			texRes->images[0].mips[0].height = cachedFont.getAtlas().getHeight();
		}

		texRes->mipCount = 1;
		texRes->type = TextureType::Texture2D;
		texRes->usage = TextureUsageType::Static;
	}

	Texture* fntTex = getGraphics().getTextures()[texture];

	if (fntTex)
	{
		fntTex->upload();
	}
}

}