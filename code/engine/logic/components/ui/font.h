// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/misc/cached_font.h"
#include "core/resource.h"

namespace engine
{
using namespace base;

enum class FontStyle
{
	Normal,
	Bold,
	Italic,
	BoldItalic
};

struct Font
{
	CachedFont cachedFont;
	ResourceId font = nullResourceId;
	FontStyle style = FontStyle::Normal;
	i32 size = 10;
	u32 usageCount = 0;
};

}