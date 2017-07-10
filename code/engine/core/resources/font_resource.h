#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/math/util.h"
#include "core/resource.h"
#include "base/math/color.h"
#include "base/misc/cached_font.h"

namespace engine
{
struct FontResource : Resource
{
	bool dynamic = false;
	u8* fontFileData = nullptr;
	u64 fontFileDataSize = 0;
	u32 glyphCount = 0;
	FontGlyph* glyphs = nullptr;
	u32 kerningPairCount = 0;
	FontKerningPair* kerningPairs = nullptr;
	ResourceId texture = nullResourceId;
};

}