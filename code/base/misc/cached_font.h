// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/dictionary.h"
#include "base/misc/image_atlas.h"

namespace base
{
class Stream;
struct ImageAtlasItem;

struct FontGlyph
{
	u32 code = 0;
	f32 bearingX = 0.0f;
	f32 advanceX = 0.0f;
	f32 advanceY = 0.0f;
	RectI pixelRect;
	RectF texRect;
	Array<u8> rgbaBuffer;
};

//! A font char-char kerning pair info
struct FontKerningPair
{
	//! left character of the kerning pair
	u32 leftCode;
	//! right character of the kerning pair
	u32 rightCode;
	//! the kerning distance value (in pixels)
	f32 kerning;
};

struct FontTextSize
{
	FontTextSize() {}
	FontTextSize(f32 w, f32 h, f32 maxDesc)
		: width(w)
		, height(h)
		, maxDescent(maxDesc)
	{}

	f32 width = 0;
	f32 height = 0;
	f32 maxDescent = 0;
};

class B_API CachedFont
{
public:
	CachedFont() {}
	CachedFont(const String& fontFilename, u32 fontFaceSize, u32 atlasSize = 1024);
	CachedFont(Stream& stream, u32 fontFaceSize, u32 atlasSize = 1024);
	~CachedFont();

	void free();
	void load(const String& fontFilename, u32 fontFaceSize, u32 atlasSize = 1024);
	void load(Stream& stream, u32 fontFaceSize, u32 atlasSize = 1024);
	FontGlyph* getGlyph(u32 glyphCode);
	f32 getKerning(u32 glyphCodeLeft, u32 glyphCodeRight);
	void precacheGlyphs(const Array<u32>& glyphCodes);
	void precacheLatinAlphabetGlyphs();
	ImageAtlasItem* getGlyphImage(u32 glyphCode);
	FontTextSize computeTextSize(const String& text);
	FontTextSize computeTextSize(const u32* text, u32 size);
	const FontTextSize& getTextSizeForAllGlyphs() const { return allGlyphsSize; }
	ImageAtlas& getAtlas() { return atlas; }

protected:
	FontGlyph* cacheGlyph(u32 glyphCode, bool packAtlasNow = false, u32 itemSpacing = 2);
	FontTextSize computeTextSizeForAllGlyphs();

	ImageAtlas atlas;
	String filename;
	u32 faceSize = 12;
	void* ftFaceRef = 0;
	Dictionary<u32 /*code*/, FontGlyph*> glyphs;
	FontTextSize allGlyphsSize;
};

}