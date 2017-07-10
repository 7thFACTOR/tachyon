#pragma once
#include "base/misc/cached_font.h"
#include "core/resource.h"

namespace engine
{
using namespace base;

//TODO: cache multiple sizes and make the CachedFont support multiple atlas textures
class ENGINE_API Font
{
public:
	Font();
	Font(ResourceId font, u32 faceSize, u32 atlasSize = 1024);
	
	void setFont(ResourceId font, u32 faceSize, u32 atlasSize = 1024);
	void setSize(u32 faceSize, u32 atlasSize = 1024);
	RectF getGlyphTexCoords(u32 code);
	CachedFont& getCachedFont() { return cachedFont; }
	ResourceId getTexture() const { return texture; }

protected:
	void updateTexture();

	CachedFont cachedFont;
	ResourceId texture = invalidResourceId;
	ResourceId font = invalidResourceId;
};

}