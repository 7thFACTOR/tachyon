#include "common.h"
#include "font_processor.h"
#include "texture_processor.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/util.h"
#include "base/json.h"
#include "base/array.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

namespace ac
{
using namespace base;

int startFreeType();
void stopFreeType();

namespace FontShadowTypes
{
	enum FontShadowType
	{
		None,
		Simple,
		Outline
	};
}
typedef FontShadowTypes::FontShadowType FontShadowType;

class FreeTypeFont
{
public:
	struct CharInfo
	{
		wchar_t charCode;
		f32 width;
		f32 height;
		f32 advance;
		f32 origin;
		int pixelWidth;
		int pixelHeight;
		int pixelX;
		int pixelY;
		char* rgbaBuffer;
	};

	struct KerningPair
	{
		wchar_t chars[2];
		f32 kerning;
	};

	void loadFont(const char* pFilename, int aPointSize);
	void free();
	bool bakeGlyph(wchar_t aChar);
	bool bakeGlyphs(const wchar_t* pChars, int aCount, unsigned int aTextColor);
	void save(
		const ArgsParser& args,
		const char* pFolder,
		const char* pName,
		int aWidth, int aHeight,
		int aGridX, int aGridY,
		int aCanvasOffsetX, int aCanvasOffsetY,
		FontShadowType aShadow,
		int aShadowOffsetX, int aShadowOffsetY,
		bool bDebug, unsigned int aShadowColor);

	const Array<CharInfo>& chars() const
	{
		return m_chars;
	}

	const Array<KerningPair>& kerningPairs() const
	{
		return m_kerningPairs;
	}

protected:
	f32 m_pointSize;
	f32 m_ascend;
	unsigned int m_textColor;
	unsigned int m_shadowColor;
	Array<CharInfo> m_chars;
	Array<KerningPair> m_kerningPairs;
	FT_Face m_pFontFace;
};

static FT_Library s_freeTypeLib;

int startFreeType()
{
	return !(FT_Init_FreeType(&s_freeTypeLib));
}

void stopFreeType()
{
	FT_Done_FreeType(s_freeTypeLib);
}

void FreeTypeFont::loadFont(const char* pFilename, int aPointSize)
{
	// load the font from the file
	if (FT_New_Face(s_freeTypeLib, pFilename, 0, &m_pFontFace))
	{
		return;
	}

	// freetype measures fonts in 64ths of pixels
	FT_Set_Char_Size(m_pFontFace, aPointSize << 6, aPointSize << 6, 96, 96);
	m_pointSize = aPointSize;
}

void FreeTypeFont::free()
{
	for (int i = 0; i < m_chars.size(); ++i)
	{
		delete [] m_chars[i].rgbaBuffer;
		m_chars[i].rgbaBuffer = 0;
	}

	FT_Done_Face(m_pFontFace);
	m_pFontFace = nullptr;
	m_chars.clear();
}

bool FreeTypeFont::bakeGlyph(wchar_t aChar)
{
	FT_Glyph glyph;

	if (FT_Load_Glyph(
			m_pFontFace,
			FT_Get_Char_Index(m_pFontFace, aChar),
			FT_LOAD_DEFAULT))
		return false;

	if (FT_Get_Glyph(m_pFontFace->glyph, &glyph))
		return false;

	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
	FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
	FT_Bitmap bitmap = bitmapGlyph->bitmap;

	int width = bitmap.width;
	int height = bitmap.rows;

	char* rgbaBuffer = new char[4 * width * height];

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			char lum = bitmap.buffer[i + bitmap.width * j];
			int index = 4 * (i + j * width);
			*((int*)&rgbaBuffer[index]) = m_textColor;
			rgbaBuffer[index + 3] = lum;
		}
	}

	CharInfo chr;

	chr.pixelWidth = width ? width : m_pFontFace->glyph->advance.x >> 6;
	chr.pixelHeight = height;
	chr.rgbaBuffer = rgbaBuffer;
	chr.charCode = aChar;
	chr.height = (f32)((m_pFontFace->glyph->metrics.horiBearingY - m_pFontFace->glyph->metrics.height) >> 6);
	chr.origin = bitmapGlyph->top - height;
	m_chars.append(chr);

	return false;
}

bool FreeTypeFont::bakeGlyphs(const wchar_t* pChars, int aCount, unsigned int aTextColor)
{
	m_textColor = aTextColor;

	for (int i = 0; i < aCount; ++i)
	{
		bakeGlyph(pChars[i]);
	}

	m_kerningPairs.clear();

	for (int i = 0; i < m_chars.size(); ++i)
	{
		for (int j = 0; j < m_chars.size(); ++j)
		{
			FT_Vector kerning;

			FT_Get_Kerning(
				m_pFontFace,
				FT_Get_Char_Index(m_pFontFace, m_chars[i].charCode),
				FT_Get_Char_Index(m_pFontFace, m_chars[j].charCode),
				FT_KERNING_DEFAULT,
				&kerning);

			if (kerning.x != 0)
			{
				KerningPair kp;

				kp.chars[0] = m_chars[i].charCode;
				kp.chars[1] = m_chars[j].charCode;
				kp.kerning = kerning.x >> 6;
				m_kerningPairs.append(kp);
			}
		}
	}

	return true;
}

//void alphaBlend(fipImage& src, fipImage& dst, int x, int y)
//{
//	int dstX, dstY;
//
//	for (int i = 0; i < src.getHeight(); ++i)
//	{
//		dstY = (y + i);
//
//		if (dstY < 0 || dstY >= dst.getHeight())
//			continue;
//
//		for (int j = 0; j < src.getWidth(); ++j)
//		{
//			dstX = x + j;
//
//			if (dstX < 0 || dstX >= dst.getWidth())
//			{
//				continue;
//			}
//
//			RGBQUAD dstPixel;
//			RGBQUAD srcPixel;
//			RGBQUAD blendedPixel;
//
//			dst.getPixelColor(dstX, dstY, &dstPixel);
//			src.getPixelColor(j, i, &srcPixel);
//
//			double front = (double)srcPixel.rgbReserved / 255.0f;
//			double back = (double)dstPixel.rgbReserved / 255.0f;
//			double ResultAlpha = front + (1.0f - front) * back;
//
//			blendedPixel.rgbRed = (BYTE)( 0.5f + (srcPixel.rgbRed * front + (1.0f - front) * dstPixel.rgbRed * back)/ResultAlpha);
//			blendedPixel.rgbGreen = (BYTE)( 0.5f + (srcPixel.rgbGreen * front + (1.0f - front) * dstPixel.rgbGreen * back)/ResultAlpha);
//			blendedPixel.rgbBlue = (BYTE)( 0.5f + (srcPixel.rgbBlue * front + (1.0f - front) * dstPixel.rgbBlue * back)/ResultAlpha);
//			blendedPixel.rgbReserved = (BYTE)(0.5f + ResultAlpha * 255.0f);
//			dst.setPixelColor(dstX, dstY, &blendedPixel);
//		}
//	}
//}

void FreeTypeFont::save(
	const ArgsParser& args,
	const char* pFolder,
	const char* pName,
	int aWidth, int aHeight,
	int aGridX, int aGridY,
	int aCanvasOffsetX, int aCanvasOffsetY,
	FontShadowType aShadow,
	int aShadowOffsetX, int aShadowOffsetY,
	bool bDebug, unsigned int aShadowColor)
{
	//fipImage img(FIT_BITMAP, aWidth, aHeight, 32);
	createFullPath(pFolder);
	//memset(img.accessPixels(), 0, aWidth * aHeight * 4);

	//int cellSizeX = aWidth / aGridX;
	//int cellSizeY = aHeight / aGridY;
	//int index = 0;
	//bool bEnd = false;

	//m_shadowColor = aShadowColor;

	//for (int i = 0; i < aGridY; ++i)
	//{
	//	for (int j = 0; j < aGridX; ++j)
	//	{
	//		if (index >= m_chars.size())
	//		{
	//			bEnd = true;
	//			break;
	//		}

	//		CharInfo& chr = m_chars[index];
	//		fipImage glyphImg(FIT_BITMAP, chr.pixelWidth, chr.pixelHeight, 32);
	//		fipImage glyphShadowImg(FIT_BITMAP, chr.pixelWidth, chr.pixelHeight, 32);
	//		int imgSize = chr.pixelWidth * chr.pixelHeight * 4;

	//		memcpy(
	//			glyphImg.accessPixels(),
	//			chr.rgbaBuffer,
	//			imgSize);
	//		glyphImg.flipVertical();

	//		if (aShadow != eShadow_None)
	//		{
	//			memcpy(
	//				glyphShadowImg.accessPixels(),
	//				chr.rgbaBuffer,
	//				imgSize);
	//			glyphShadowImg.flipVertical();

	//			BYTE* pPixels = glyphShadowImg.accessPixels();
	//			BYTE alpha;
	//			int numPixels = chr.pixelWidth * chr.pixelHeight;
	//			
	//			while (numPixels > 0)
	//			{
	//				alpha = *(pPixels + 3);
	//				*((int*)pPixels) = m_shadowColor;
	//				*(pPixels + 3) = alpha;
	//				--numPixels;
	//				pPixels += 4;
	//			}
	//		}

	//		if (bDebug)
	//		{
	//			RGBQUAD c;
	//			c.rgbRed = 0;
	//			c.rgbGreen = 255;
	//			c.rgbBlue = 255;
	//			c.rgbReserved = 255;
	//			glyphImg.setPixelColor(0, 0, &c);
	//		}

	//		int x = j * cellSizeX + aCanvasOffsetX;
	//		int y = i * cellSizeY + aCanvasOffsetY;
	//		int finalX = x, finalY = y;

	//		chr.pixelX = x;
	//		chr.pixelY = y;

	//		if (aShadow != eShadow_None)
	//		{
	//			switch (aShadow)
	//			{
	//			case eShadow_Simple:
	//				{
	//					alphaBlend(glyphShadowImg, img, finalX + aShadowOffsetX, finalY + aShadowOffsetY);

	//					if (aShadowOffsetX > 0)
	//					{
	//						chr.pixelWidth += aShadowOffsetX;
	//					}
	//					else
	//					{
	//						chr.pixelX += aShadowOffsetX;
	//						chr.pixelWidth += abs(aShadowOffsetX);
	//					}

	//					if (aShadowOffsetY > 0)
	//					{
	//						chr.pixelHeight += aShadowOffsetY;
	//						chr.pixelY -= aShadowOffsetY;
	//					}
	//					else
	//					{
	//						chr.pixelHeight += abs(aShadowOffsetY);
	//						chr.pixelY -= abs(aShadowOffsetY);
	//						chr.origin -= aShadowOffsetY;
	//					}

	//					break;
	//				}
	//			case eShadow_Outline:
	//				{
	//					alphaBlend(glyphShadowImg, img, finalX + aShadowOffsetX, finalY + aShadowOffsetY);
	//					alphaBlend(glyphShadowImg, img, finalX - aShadowOffsetX, finalY - aShadowOffsetY);
	//					alphaBlend(glyphShadowImg, img, finalX - aShadowOffsetX, finalY + aShadowOffsetY);
	//					alphaBlend(glyphShadowImg, img, finalX + aShadowOffsetX, finalY - aShadowOffsetY);
	//					alphaBlend(glyphShadowImg, img, finalX + aShadowOffsetX, finalY);
	//					alphaBlend(glyphShadowImg, img, finalX - aShadowOffsetX, finalY);
	//					alphaBlend(glyphShadowImg, img, finalX, finalY + aShadowOffsetY);
	//					alphaBlend(glyphShadowImg, img, finalX, finalY - aShadowOffsetY);

	//					chr.pixelX -= aShadowOffsetX;
	//					chr.pixelY -= aShadowOffsetY;
	//					chr.pixelWidth += aShadowOffsetX * 2;
	//					chr.pixelHeight += aShadowOffsetY * 2;
	//					chr.origin -= aShadowOffsetY * 2;
	//					break;
	//				}
	//			}
	//		}

	//		alphaBlend(glyphImg, img, finalX, finalY);

	//		if (bDebug)
	//		{
	//			RGBQUAD c;
	//			c.rgbRed = 255;
	//			c.rgbGreen = 0;
	//			c.rgbBlue = 0;
	//			c.rgbReserved = 255;
	//			img.setPixelColor(x, y, &c);

	//			c.rgbRed = 255;
	//			c.rgbGreen = 255;
	//			c.rgbBlue = 0;
	//			c.rgbReserved = 255;
	//			img.setPixelColor(chr.pixelX, chr.pixelY, &c);

	//			c.rgbRed = 0;
	//			c.rgbGreen = 255;
	//			c.rgbBlue = 0;
	//			c.rgbReserved = 255;
	//			img.setPixelColor(chr.pixelX, chr.pixelY + chr.origin, &c);
	//		}

	//		++index;
	//	}

	//	if (bEnd)
	//	{
	//		break;
	//	}
	//}

	String filename;

	mergePathFileExtension(pFolder, pName, ".png", filename);
	//img.save(filename.c_str());
	
	TextureProcessor texcompiler;

	texcompiler.process(filename.c_str(), project);
	deleteFile(filename.c_str());
}

//---

FontProcessor::FontProcessor()
{
	startFreeType();
}

FontProcessor::~FontProcessor()
{
	stopFreeType();
}

const char* FontProcessor::supportedFileExtensions() const
{
	return ".font";
}

bool FontProcessor::process(const char* pSrcFilename, const Project& project)
{
	String filename, nameOnly;
	FileWriter file;
	JsonDocument doc;

	if (!doc.loadAndParse(pSrcFilename))
	{
		return false;
	}

	extractFileNameNoExtension(pSrcFilename, nameOnly);
	mergePathFileExtension(pDestPath, nameOnly.c_str(), ".font", filename);

	if (isFilesLastTimeSame(pSrcFilename, filename.c_str()))
	{
		B_LOG_DEBUG("Skipping %s", pSrcFilename);
		return true;
	}

	bool bOk = file.openFile(filename.c_str());
	B_ASSERT(bOk);

	if (!bOk)
	{
		return false;
	}
		
	FreeTypeFont font;

	String fontFile = doc.findStringValue("font", "c:/windows/fonts/arial.ttf");
	String chars = doc.findStringValue("chars", " abcdefghijklmnoprstuvwxyzABCDEFGHIJKLMNOPRSTUVWXYZ1234567890!@#$%^&*()_+~`[]{}:;\"'\\|,./<>?");
	int pointSize = doc.findIntValue("size", 15);
	int startChar = doc.findIntValue("start", 32);
	int endChar = doc.findIntValue("end", 255);
	int texWidth = doc.findIntValue("width", 512);
	int texHeight = doc.findIntValue("height", 512);
	int gridX = doc.findIntValue("gridx", 16);
	int gridY = doc.findIntValue("gridy", 16);
	int offsX = doc.findIntValue("xoffset", 3);
	int offsY = doc.findIntValue("yoffset", 3);
	int shadowOffsX = doc.findIntValue("shadow_xoffset", 1);
	int shadowOffsY = doc.findIntValue("shadow_yoffset", 1);
	int shadow = doc.findIntValue("shadow_type", 0);
	unsigned int textColor = 0xffffff;
	unsigned int shadowColor = 0;
	
	sscanf_s(doc.findStringValue("text_color", "ffffffff"), "%x", &textColor);
	sscanf_s(doc.findStringValue("shadow_color", "0"), "%x", &shadowColor);

	bool bDebug = doc.findBoolValue("debug");

	wchar_t* wchars = nullptr;
	int numWChars = 0;

	if (doc.findBoolValue("use_range"))
	{
		numWChars = endChar - startChar;
		
		if (numWChars < 1)
		{
			B_LOG_ERROR("Invalid characters range: %d - %d", startChar, endChar);
			exit(0);
		}
		
		wchars = new wchar_t[numWChars];

		for (int i = 0; i < numWChars; ++i)
		{
			wchars[i] = startChar + i;
		}
	}
	else
	{
		numWChars = chars.length();

		if (numWChars < 1)
		{
			B_LOG_ERROR("Invalid character count: %d", numWChars);
			exit(0);
		}

		wchars = new wchar_t[numWChars];

		for (int i = 0; i < numWChars; ++i)
		{
			wchars[i] = chars[i];
		}
	}

	font.loadFont(fontFile.c_str(), pointSize);
	font.bakeGlyphs(wchars, numWChars, textColor);
	font.save(
		rArgs,
		pDestPath,
		nameOnly.c_str(),
		texWidth,
		texHeight,
		gridX,
		gridY,
		offsX,
		offsY,
		(FontShadowType)shadow,
		shadowOffsX,
		shadowOffsY,
		bDebug,
		shadowColor);
	
	//file.beginChunk(eFontChunk_Header, 1);
	//file.writeString("FONT");
	//file.endChunk();

	//file.beginChunk(eFontChunk_Chars, 1);

	int charCount = font.chars().size();

	file.writeInt32(charCount);

	for (size_t i = 0; i < font.chars().size(); ++i)
	{
		const FreeTypeFont::CharInfo& charInfo = font.chars()[i];

		int pixelX = charInfo.pixelX;
		int pixelY = charInfo.pixelY;
		int pixelWidth = charInfo.pixelWidth;
		int pixelHeight = charInfo.pixelHeight;
		int pixelOrigin = charInfo.origin - pixelHeight;

		pixelY = texHeight - pixelY;

		file.writeInt16(charInfo.charCode);
		file.writeInt16(pixelX);
		file.writeInt16(pixelY);
		file.writeInt16(pixelWidth);
		file.writeInt16(pixelHeight);
		file.writeInt16(pixelOrigin);
		file.writeFloat((f32)pixelX / texWidth);
		file.writeFloat((f32)pixelY / texHeight - (f32)pixelHeight / texHeight);
		file.writeFloat((f32)pixelWidth / texWidth);
		file.writeFloat((f32)pixelHeight / texHeight);
		file.writeFloat((f32)pixelOrigin / texHeight);
	}

	file.endChunk();

	//file.beginChunk(eFontChunk_Kernings, 1);

	int kerningCount = font.kerningPairs().size();

	file.writeInt32(kerningCount);

	for (size_t i = 0; i < font.kerningPairs().size(); ++i)
	{
		const FreeTypeFont::KerningPair& kPair = font.kerningPairs()[i];

		file.writeInt16(kPair.chars[0]);
		file.writeInt16(kPair.chars[1]);
		file.writeInt16(kPair.kerning);
	}

	file.endChunk();
	file.closeFile();
	setSameFileTime(pSrcFilename, filename.c_str());

	return true;
}
}