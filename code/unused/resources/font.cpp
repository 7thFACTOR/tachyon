#include <string.h>
#include "resources/font.h"
#include "base/util.h"
#include "core/core.h"
#include "base/logger.h"
#include "base/assert.h"
#include "core/resource_manager.h"
#include "resources/texture.h"
#include "render/geometry_buffer.h"
#include "render/texcoord.h"

namespace engine
{
Font::Font()
{
	m_texture = kInvalidResourceId;
}

Font::~Font()
{
}

bool Font::load(Stream* pStream)
{
	Resource::load(pStream);
	String str;

	while (!pStream->eof())
	{
		ChunkHeader chunk;

		if (!pStream->readChunkHeader(&chunk))
		{
			break;
		}

		switch (chunk.id)
		{
		case FontChunkIds::Header:
			{
				pStream->readUint32(m_texture);
				resources().load(m_texture);
				break;
			}
		case FontChunkIds::Chars:
			{
				i32 charCount = 0;

				pStream->readInt32(charCount);
				B_LOG_DEBUG("Char count: %d", charCount);

				for (i32 i = 0; i < charCount; ++i)
				{
					FontGlyph character;
					i16 x = 0, y = 0, w = 0, h = 0, chr = 0, orig = 0;

					pStream->readInt16(chr);
					pStream->readInt16(x);
					pStream->readInt16(y);
					pStream->readInt16(w);
					pStream->readInt16(h);
					pStream->readInt16(orig);
					pStream->readFloat(character.uvX);
					pStream->readFloat(character.uvY);
					pStream->readFloat(character.uvWidth);
					pStream->readFloat(character.uvHeight);
					pStream->readFloat(character.uvOrigin);
					character.glyphCode = chr;
					character.x = x;
					character.y = y;
					character.width = w;
					character.height = h;
					character.origin = orig;
					m_glyphs.append(character);
				}
				break;
			}

		case FontChunkIds::Kernings:
			{
				i32 pairCount = 0;

				pStream->readInt32(pairCount);

				for (i32 i = 0; i < pairCount; ++i)
				{
					FontKerningPair pair;
					i16 chr1, chr2, kerning;

					pStream->readInt16(chr1);
					pStream->readInt16(chr2);
					pStream->readInt16(kerning);
					pair.kerning = kerning;
					pair.leftGlyphCode = chr1;
					pair.rightGlyphCode = chr2;
					m_glyphKerningPairs.append(pair);
				}
				break;
			}
		default:
			pStream->ignoreChunk(&chunk);
		}
	}

	return true;
}

bool Font::unload()
{
	m_glyphs.clear();
	m_glyphKerningPairs.clear();

	if (m_texture)
	{
		resources().release(m_texture);
	}
	
	m_texture = kInvalidResourceId;

	return true;
}

FontGlyph* Font::glyphByCode(wchar_t aCode)
{
	for (size_t i = 0, iCount = m_glyphs.size(); i < iCount; ++i)
	{
		if (aCode == m_glyphs[i].glyphCode)
		{
			return &m_glyphs[i];
		}
	}

	return nullptr;
}

FontKerningPair* Font::kerningPairByGlyphCodes(wchar_t aLeftCharCode, wchar_t aRightCharCode)
{
	for (size_t i = 0, iCount = m_glyphKerningPairs.size(); i < iCount; ++i)
	{
		if (aLeftCharCode == m_glyphKerningPairs[i].leftGlyphCode &&
			aRightCharCode == m_glyphKerningPairs[i].rightGlyphCode)
		{
			return &m_glyphKerningPairs[i];
		}
	}

	return nullptr;
}

const Array<FontGlyph>& Font::glyphs() const
{
	return m_glyphs;
}

const Array<FontKerningPair>& Font::glyphKerningPairs() const
{
	return m_glyphKerningPairs;
}

ResourceId Font::texture() const
{
	return m_texture;
}

void Font::computeTextRectangle(const char* pText, f32& rX, f32& rY, f32& rWidth, f32& rHeight, f32 aTextCharSpacing)
{
	rX = rY = rWidth = rHeight = 0;

	if (!strcmp(pText, ""))
	{
		return;
	}

	u32 numChars = strlen(pText);
	u32 kerning = 0;
	FontGlyph* pChar = nullptr;
	FontKerningPair* pKPair = nullptr;

	for (u32 i = 0; i < numChars; ++i)
	{
		pChar = glyphByCode(pText[i]);
		kerning = 0;

		if (i < numChars - 1)
		{
			pKPair = kerningPairByGlyphCodes(pText[i], pText[i + 1]);

			if (pKPair)
			{
				kerning = pKPair->kerning;
			}
		}

		if (!pChar)
		{
			continue;
		}

		rWidth += pChar->width + aTextCharSpacing + kerning;

		if (rHeight < pChar->height)
		{
			rHeight = pChar->height;
		}
	}
}
}