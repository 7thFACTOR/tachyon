#include "base/misc/cached_font.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include "base/stream.h"
#include "base/util.h"

namespace base
{
static FT_Library s_freeTypeLib = nullptr;
static u32 s_freeTypeUsageCount = 0;

bool startFreeType()
{
	if (!s_freeTypeUsageCount)
	{
		s_freeTypeUsageCount++;
		return !(FT_Init_FreeType(&s_freeTypeLib));
	}

	s_freeTypeUsageCount++;

	return true;
}

void stopFreeType()
{
	s_freeTypeUsageCount--;

	if (s_freeTypeUsageCount <= 0 && s_freeTypeLib)
	{
		FT_Done_FreeType(s_freeTypeLib);
		s_freeTypeLib = nullptr;
	}
}

CachedFont::CachedFont(const String& fontFilename, u32 faceSize, u32 atlasSize)
{
	load(fontFilename, faceSize, atlasSize);
}

CachedFont::CachedFont(Stream& stream, u32 faceSize, u32 atlasSize)
{
	load(stream, faceSize, atlasSize);
}

void CachedFont::free()
{
	atlas.clearItems();
	atlas.clearPixels();

	for (auto glyph : glyphs)
	{
		delete glyph.value;
	}

	if (ftFaceRef)
	{
		FT_Done_Face((FT_Face)ftFaceRef);
	}

	stopFreeType();
}

void CachedFont::load(const String& fontFilename, u32 facePointSize, u32 atlasSize)
{
	atlas.clearItems();
	atlas.resize(atlasSize, atlasSize);
	startFreeType();
	filename = fontFilename;
	faceSize = facePointSize;
	ftFaceRef = new FT_Face();

	// load the font from the file
	if (FT_New_Face(s_freeTypeLib, fontFilename.c_str(), 0, (FT_Face*)&ftFaceRef))
	{
		return;
	}

	// freetype measures fonts in 64ths of pixels
	FT_Set_Char_Size((FT_Face)ftFaceRef, faceSize << 6, faceSize << 6, 96, 96);
}

void CachedFont::load(Stream& stream, u32 facePointSize, u32 atlasSize)
{
	atlas.clearItems();
	atlas.resize(atlasSize, atlasSize);
	startFreeType();

	filename = "";
	faceSize = facePointSize;
	ftFaceRef = new FT_Face();

	u64 fontDataSize = stream.getSize();
	Array<u8> fontData;

	if (!stream.read((void*)fontData.data(), fontDataSize))
	{
		stopFreeType();
		return;
	}

	fontData.resize(fontDataSize);

	// load the font from the file
	if (FT_New_Memory_Face(s_freeTypeLib, fontData.data(), fontData.size(), 0, (FT_Face*)&ftFaceRef))
	{
		return;
	}

	// freetype measures fonts in 64ths of pixels
	FT_Set_Char_Size((FT_Face)ftFaceRef, faceSize << 6, faceSize << 6, 96, 96);
}

CachedFont::~CachedFont()
{
	free();
}

FontGlyph* CachedFont::getGlyph(u32 glyphCode)
{
	auto iter = glyphs.find(glyphCode);

	// glyph not cached, do it
	if (iter == glyphs.end())
	{
		return cacheGlyph(glyphCode, true);
	}

	return iter->value;
}

f32 CachedFont::getKerning(u32 glyphCodeLeft, u32 glyphCodeRight)
{
	FT_Vector kerning;

	if (FT_Get_Kerning(
		(FT_Face)ftFaceRef,
		FT_Get_Char_Index((FT_Face)ftFaceRef, glyphCodeLeft),
		FT_Get_Char_Index((FT_Face)ftFaceRef, glyphCodeRight),
		FT_KERNING_DEFAULT,
		&kerning))
	{
		return 0.0f;
	}

	if (kerning.x != 0)
	{
		return (f32)(kerning.x >> 6);
	}

	return 0.0f;
}

void CachedFont::precacheGlyphs(const Array<u32>& glyphCodes)
{
	for (auto glyphCode : glyphCodes)
	{
		cacheGlyph(glyphCode);
	}

	atlas.pack(2, Color(1,1,1,0));
}

void CachedFont::precacheLatinAlphabetGlyphs()
{
	String latinAlphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=~`[]{};':\",./<>?ой ";

	for (auto code : latinAlphabet)
	{
		cacheGlyph((u8)code);
	}

	cacheGlyph(0x2122);
	atlas.pack(2, Color(1, 1, 1, 0));
	allGlyphsSize = computeTextSizeForAllGlyphs();
}

FontGlyph* CachedFont::cacheGlyph(u32 glyphCode, bool packAtlasNow, u32 itemSpacing)
{
	if (!ftFaceRef)
	{
		return nullptr;
	}

	FontGlyph* fontGlyph = new FontGlyph();
	FT_Glyph glyph;

	if (FT_Load_Glyph(
		(FT_Face)ftFaceRef,
		FT_Get_Char_Index((FT_Face)ftFaceRef, glyphCode),
		FT_LOAD_DEFAULT))
	{
		return nullptr;
	}

	if (FT_Get_Glyph(((FT_Face)ftFaceRef)->glyph, &glyph))
	{
		return nullptr;
	}

	if (FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1))
	{
		return nullptr;
	}

	FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
	FT_Bitmap bitmap = bitmapGlyph->bitmap;
	u32 width = bitmap.width;
	u32 height = bitmap.rows;
	Array<u8> rgbaBuffer;

	rgbaBuffer.resize(4 * width * height);

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			u8 lum = bitmap.buffer[i + bitmap.width * j];
			u32 index = 4 * (i + j * width);
			*(u32*)(rgbaBuffer.data() + index) = ~0;
			rgbaBuffer[index + 3] = lum;
		}
	}

	fontGlyph->pixelRect.width = width;
	fontGlyph->pixelRect.height = height;
	fontGlyph->rgbaBuffer = rgbaBuffer;
	fontGlyph->code = glyphCode;
	fontGlyph->advanceX = (f32)(((FT_Face)ftFaceRef)->glyph->advance.x >> 6);
	fontGlyph->advanceY = (f32)bitmapGlyph->top;
	fontGlyph->bearingX = (f32)(((FT_Face)ftFaceRef)->glyph->metrics.horiBearingX >> 6);
	
	glyphs.add(glyphCode, fontGlyph);

	atlas.addItem(
		toString(glyphCode),
		(u32*)fontGlyph->rgbaBuffer.data(),
		fontGlyph->pixelRect.width,
		fontGlyph->pixelRect.height);

	if (packAtlasNow)
	{
		atlas.pack(itemSpacing, Color(1, 1, 1, 0));
		allGlyphsSize = computeTextSizeForAllGlyphs();
	}

	return fontGlyph;
}

ImageAtlasItem* CachedFont::getGlyphImage(u32 glyphCode)
{
	return atlas.getItem(toString(glyphCode));
}

FontTextSize CachedFont::computeTextSize(const String& text)
{
	FontTextSize fsize;
	u32 lastChr = 0;
	f32 minY = 1000.0f, maxY =-1000.0f;
	fsize.maxDescent = 0.0f;

	for (auto chr : text)
	{
		auto glyphSprite = getGlyphImage(chr);
		auto glyph = getGlyph(chr);

		if (glyphSprite && glyph)
		{
			f32 gyOffs = glyph->pixelRect.height - glyph->advanceY;

			if (fsize.maxDescent < gyOffs)
			{
				fsize.maxDescent = gyOffs;
			}

			f32 gyMax = (f32)glyph->pixelRect.height;
			f32 gyMin = 0;

			gyMax -= gyOffs;
			gyMin -= gyOffs;

			if (minY > gyMin)
			{
				minY = gyMin;
			}

			if (maxY < gyMax)
			{
				maxY = gyMax;
			}

			auto kern = getKerning(lastChr, chr);
			fsize.width += glyph->advanceX + kern;
			lastChr = chr;
		}
	}

	fsize.height = maxY - minY;

	return fsize;
}

FontTextSize CachedFont::computeTextSizeForAllGlyphs()
{
	FontTextSize fsize;
	u32 lastChr = 0;
	f32 minY = 1000.0f, maxY = -1000.0f;
	fsize.maxDescent = 0.0f;

	for (auto& glyphPair : glyphs)
	{
		FontGlyph* glyph = glyphPair.value;
		auto glyphSprite = getGlyphImage(glyph->code);

		if (glyphSprite)
		{
			f32 gyOffs = glyph->pixelRect.height - glyph->advanceY;

			if (fsize.maxDescent < gyOffs)
			{
				fsize.maxDescent = gyOffs;
			}

			f32 gyMax = (f32)glyph->pixelRect.height;
			f32 gyMin = 0;

			gyMax -= gyOffs;
			gyMin -= gyOffs;

			if (minY > gyMin)
			{
				minY = gyMin;
			}

			if (maxY < gyMax)
			{
				maxY = gyMax;
			}

			auto kern = getKerning(lastChr, glyph->code);
			fsize.width += glyph->advanceX + kern;
			lastChr = glyph->code;
		}
	}

	fsize.height = maxY - minY;

	return fsize;
}

}