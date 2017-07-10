#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/math/util.h"
#include "core/resource.h"
#include "base/math/color.h"
#include "render/render_manager.h"

namespace engine
{
class Texture;

namespace FontChunkIds
{
	enum FontChunkId
	{
		Header,
		Chars,
		Kernings
	};
}
typedef FontChunkIds::FontChunkId FontChunkId;

//! A font char info
struct FontGlyph
{
	//! the font's character code
	wchar_t glyphCode;
	//! the X position of the character in the font texture in pixels
	i32 x;
	//! the Y position of the character in the font texture in pixels
	i32 y;
	//! the width of the character in the font texture in pixels
	i32 width;
	//! the height of the character in the font texture in pixels
	i32 height;
	i32 origin;
	//! the X position of the character in the font texture in 0..1 range
	f32 uvX;
	//! the Y position of the character in the font texture in 0..1 range
	f32 uvY;
	//! the width of the character in the font texture in 0..1 range
	f32 uvWidth;
	//! the height of the character in the font texture in 0..1 range
	f32 uvHeight;
	f32 uvOrigin;
};

//! A font char-char kerning pair info
struct FontKerningPair
{
	//! left character of the kerning pair
	wchar_t leftGlyphCode;
	//! right character of the kerning pair
	wchar_t rightGlyphCode;
	//! the kerning distance value (in pixels)
	f32 kerning;
};

//! The font data as loaded from .fnt.xml files
class E_API Font : public Resource
{
public:
	B_RUNTIME_CLASS;
	Font();
	virtual ~Font();
	//! load the font data from XML file and texture
	bool load(Stream* pStream);
	//! unload the font data
	bool unload();
	//! \return the character data by its ASCII/UNICODE code
	FontGlyph* glyphByCode(wchar_t aCode);
	//! \return the kerning pair by the two close left-right char codes
	FontKerningPair* kerningPairByGlyphCodes(wchar_t aLeftCharCode, wchar_t aRightCharCode);
	//! \return the characters data array
	const Array<FontGlyph>& glyphs() const;
	//! \return the characters kerning pairs
	const Array<FontKerningPair>& glyphKerningPairs() const;
	//! \return the font's texture map
	ResourceId texture() const;
	void computeTextRectangle(
		const char* pText,
		f32& rX, f32& rY,
		f32& rWidth, f32& rHeight,
		f32 aTextCharSpacing);

protected:
	//! the characters array
	Array<FontGlyph> m_glyphs;
	//! the characters kerning pairs
	Array<FontKerningPair> m_glyphKerningPairs;
	//! the font texture
	ResourceId m_texture;
};
}