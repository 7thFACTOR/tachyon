#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/array.h"
#include "logic/components/ui/font.h"

namespace engine
{
using namespace base;

class FontCache
{
public:
	FontCache();
	~FontCache();
	Font* createFont(ResourceId font, FontStyle style, i32 size);
	void releaseFont(Font* font);
	void deleteFonts();

protected:
	Array<Font*> cachedFonts;
};

}