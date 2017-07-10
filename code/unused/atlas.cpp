#include <string.h>
#include <horus/atlas.h>
#include <horus/canvas.h>
#include <binpack/Rect.h>
#include <binpack/GuillotineBinPack.h>
#include <binpack/ShelfBinPack.h>
#include <binpack/SkylineBinPack.h>
#include <binpack/MaxRectsBinPack.h>

namespace horus
{
Atlas::Atlas(u32 width, u32 height)
{
	texture.resize(width, height);
}

Sprite* Atlas::spriteById(SpriteId id) const
{
	auto iter = sprites.find(id);

	if (iter == sprites.end())
	{
		return nullptr;
	}

	return iter->second;
}

void Atlas::addSprite(SpriteId id, u8* imageData, u32 width, u32 height)
{
	PackSpriteData psd;

	u32 imageSize = width*height * 4;
	psd.imageData = new u8[imageSize];
	memcpy(psd.imageData, imageData, imageSize);
	psd.id = id;
	psd.width = width;
	psd.height = height;

	packSprites.push_back(psd);
}

SpriteId Atlas::addWhiteSprite(SpriteId id, u32 width)
{
	u32 whiteImageSize = width * width * 4;
	u8* whiteImage = new u8[whiteImageSize];

	memset(whiteImage, ~0, whiteImageSize);
	addSprite(id, whiteImage, width, width);

	return id;
}

void Atlas::pack(
	u32 spriteBorderSize,
	const Color& fillColor,
	AtlasPackPolicy packing)
{
	u32 border2 = spriteBorderSize * 2;
	bool useWasteMap = true;//TODO: add as argument to method ?
	::Rect packedRect;
	bool rotated = false;

	switch (packing)
	{
	case horus::AtlasPackPolicy::Guillotine:
	{
		GuillotineBinPack guillotineBinPack;
		
		guillotineBinPack.Init(texture.getWidth(), texture.getHeight());

		for (auto& packSprite : packSprites)
		{
			packedRect = guillotineBinPack.Insert(
				packSprite.width + border2,
				packSprite.height + border2,
				true,
				GuillotineBinPack::FreeRectChoiceHeuristic::RectBestShortSideFit,
				GuillotineBinPack::GuillotineSplitHeuristic::SplitLongerAxis);

			packSprite.packedRect.x = packedRect.x;
			packSprite.packedRect.y = packedRect.y;
			packSprite.packedRect.width = packedRect.width;
			packSprite.packedRect.height = packedRect.height;
		}

		break;
	}
	case horus::AtlasPackPolicy::MaxRects:
	{
		MaxRectsBinPack maxRectsBinPack;

		maxRectsBinPack.Init(texture.getWidth(), texture.getHeight());

		for (auto& packSprite : packSprites)
		{
			packedRect = maxRectsBinPack.Insert(
				packSprite.width + border2,
				packSprite.height + border2,
				MaxRectsBinPack::FreeRectChoiceHeuristic::RectBestAreaFit);

			packSprite.packedRect.x = packedRect.x;
			packSprite.packedRect.y = packedRect.y;
			packSprite.packedRect.width = packedRect.width;
			packSprite.packedRect.height = packedRect.height;
		}

		break;
	}
	case horus::AtlasPackPolicy::ShelfBin:
	{
		ShelfBinPack shelfBinPack;
		
		shelfBinPack.Init(texture.getWidth(), texture.getHeight(), useWasteMap);

		for (auto& packSprite : packSprites)
		{
			packedRect = shelfBinPack.Insert(
				packSprite.width + border2,
				packSprite.height + border2,
				ShelfBinPack::ShelfChoiceHeuristic::ShelfBestAreaFit);

			packSprite.packedRect.x = packedRect.x;
			packSprite.packedRect.y = packedRect.y;
			packSprite.packedRect.width = packedRect.width;
			packSprite.packedRect.height = packedRect.height;
		}

		break;
	}
	case horus::AtlasPackPolicy::Skyline:
	{
		SkylineBinPack skylineBinPack;

		skylineBinPack.Init(texture.getWidth(), texture.getHeight(), useWasteMap);

		for (auto& packSprite : packSprites)
		{
			packedRect = skylineBinPack.Insert(
				packSprite.width + border2,
				packSprite.height + border2,
				SkylineBinPack::LevelChoiceHeuristic::LevelMinWasteFit);

			packSprite.packedRect.x = packedRect.x;
			packSprite.packedRect.y = packedRect.y;
			packSprite.packedRect.width = packedRect.width;
			packSprite.packedRect.height = packedRect.height;
		}

		break;
	}
	default:
		break;
	}

	// we have now the rects inside the atlas
	// create texture data and update the api texture
	u32 atlasImageDataSize = texture.getWidth() * texture.getHeight();

	if (!atlasImageDataSize)
	{
		return;
	}

	u32* atlasImageData = new u32[atlasImageDataSize];

	if (!atlasImageData)
	{
		return;
	}

	if (fillColor.rgba() == 0)
	{
		memset(atlasImageData, 0, atlasImageDataSize * 4);
	}
	else
	{
		u32 col = fillColor.rgba();

		for (int i = 0; i < atlasImageDataSize; i++)
		{
			atlasImageData[i] = col;
		}
	}

	//TODO: hmmm, lets do this a little more optimized? only add to atlas pending sprites
	// for now we just do it bruteforce
	for (auto& packSprite : packSprites)
	{
		Sprite* sprite = new Sprite;

		packSprite.packedRect.x += spriteBorderSize;
		packSprite.packedRect.y += spriteBorderSize;
		packSprite.packedRect.width -= border2;
		packSprite.packedRect.height -= border2;

		sprite->atlas = this;
		sprite->width = packSprite.width;
		sprite->height = packSprite.height;
		sprite->rotated = packSprite.width != packSprite.packedRect.width;

		sprite->texCoords.set(
			(f32)packSprite.packedRect.x / (f32)texture.getWidth(),
			(f32)packSprite.packedRect.y / (f32)texture.getHeight() - 0.0002f,
			(f32)packSprite.packedRect.width / (f32)texture.getWidth(),
			(f32)packSprite.packedRect.height / (f32)texture.getHeight() + 0.0001f);

		sprites.insert(std::make_pair(packSprite.id, sprite));

		u32 texWidth = texture.getWidth();

		// copy sprite image to the atlas image buffer
		if (sprite->rotated)
		{
			for (u32 y = 0; y < packSprite.height; y++)
			{
				for (u32 x = 0; x < packSprite.width; x++)
				{
					u32 destIndex =
						packSprite.packedRect.x + y + (packSprite.packedRect.y + x) * texWidth;
					u32 srcIndex = y * packSprite.width + (packSprite.width - 1) - x;
					atlasImageData[destIndex] = ((u32*)packSprite.imageData)[srcIndex];
				}
			}
		}
		else
		for (u32 y = 0; y < packSprite.height; y++)
		{
			for (u32 x = 0; x < packSprite.width; x++)
			{
				u32 destIndex =
					packSprite.packedRect.x + x + (packSprite.packedRect.y + y) * texWidth;
				u32 srcIndex = x + y * packSprite.width;
				atlasImageData[destIndex] = ((u32*)packSprite.imageData)[srcIndex];
			}
		}
	}

	texture.updateData(atlasImageData);
	delete[] atlasImageData;//TODO: maybe we can keep this in the class as a member
}

void Atlas::clearSprites()
{
	for (auto sprite : sprites)
	{
		delete sprite.second;
	}

	for (auto& sprite : packSprites)
	{
		delete[] sprite.imageData;
	}

	sprites.clear();
	packSprites.clear();
}

}