// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/memory_stream.h"
#include "core/globals.h"
#include "core/resource_repository.h"
#include "core/resources/font_resource.h"
#include "logic/component_updaters/ui/font_cache.h"
#include "logic/components/ui/font.h"

namespace engine
{
FontCache::FontCache()
{
}

FontCache::~FontCache()
{
	deleteFonts();
}

Font* FontCache::createFont(ResourceId font, FontStyle style, i32 size)
{
	// search it first
	for (auto fnt : cachedFonts)
	{
		if (fnt->font == font
			&& fnt->style == style
			&& fnt->size == size)
		{
			fnt->usageCount++;
			return fnt;
		}
	}

	ScopedResourceMapping<FontResource> fntData(font);

	if (!fntData->dynamic)
	{
		B_ASSERT(!"The font is not dynamic, so it doesnt have font data inside it, please mark it dynamic in the asset meta");
		return nullptr;
	}

	// create a new one
	Font* newFont = new Font();

	B_ASSERT(newFont);

	MemoryStream memStream(fntData->fontFileData, fntData->fontFileDataSize);
	newFont->cachedFont.load(memStream, size);
	newFont->cachedFont.precacheLatinAlphabetGlyphs();
	newFont->style = style;
	newFont->size = size;
	newFont->font = font;
	newFont->usageCount = 1;

	cachedFonts.appendUnique(newFont);

	return newFont;
}

void FontCache::releaseFont(Font* font)
{
	B_ASSERT(font);

	font->usageCount--;

	//TODO: delete font with usage 0 ? might free RAM and VRAM
}

void FontCache::deleteFonts()
{
	for (auto font : cachedFonts)
	{
		font->cachedFont.free();
	}

	cachedFonts.clear();
}

}