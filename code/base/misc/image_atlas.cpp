#include <string.h>
#include "base/misc/image_atlas.h"
#include <3rdparty/binpack/Rect.h>
#include <3rdparty/binpack/GuillotineBinPack.h>
#include <3rdparty/binpack/ShelfBinPack.h>
#include <3rdparty/binpack/SkylineBinPack.h>
#include <3rdparty/binpack/MaxRectsBinPack.h>
#include "base/util.h"
#include "base/logger.h"

namespace base
{
ImageAtlas::ImageAtlas(u32 width, u32 height)
{
	resize(width, height);
}

void ImageAtlas::resize(u32 width, u32 height)
{
	atlasWidth = width;
	atlasHeight = height;
	pixels.resize(width * height);
}

ImageAtlasItem* ImageAtlas::getItem(const String& name)
{
	auto iter = items.find(name);

	if (iter == items.end())
	{
		return nullptr;
	}

	return iter->value;
}

void ImageAtlas::addItem(const String& name, u32* imageData, u32 width, u32 height)
{
	ImageAtlasItem* item = new ImageAtlasItem;

	u32 imageSize = width * height;
	item->imageData = new u32[imageSize];
	memcpy(item->imageData, imageData, imageSize * 4);
	item->name = name;
	item->width = width;
	item->height = height;

	items.add(name, item);
}

void ImageAtlas::addWhiteItem(u32 width)
{
	u32 whiteImageSize = width * width;
	u32* whiteImage = new u32[whiteImageSize];

	memset(whiteImage, ~0, whiteImageSize * 4);
	addItem("white", whiteImage, width, width);
}

bool ImageAtlas::pack(
	u32 itemBorderSize,
	const Color& fillColor,
	ImageAtlasPackPolicy packing)
{
	u32 border2 = itemBorderSize * 2;
	bool useWasteMap = true;//TODO: add as argument to method ?
	::Rect packedRect;
	bool rotated = false;

	switch (packing)
	{
	case ImageAtlasPackPolicy::Guillotine:
	{
		GuillotineBinPack guillotineBinPack;
		
		guillotineBinPack.Init(atlasWidth, atlasHeight);

		for (auto item : items)
		{
			packedRect = guillotineBinPack.Insert(
				item.value->width + border2,
				item.value->height + border2,
				true,
				GuillotineBinPack::FreeRectChoiceHeuristic::RectBestShortSideFit,
				GuillotineBinPack::GuillotineSplitHeuristic::SplitLongerAxis);

			if (packedRect.width == 0 && packedRect.height == 0)
			{
				B_LOG_ERROR("No room for images into the atlas, make atlas bigger");
				return false;
			}

			item.value->packedRect.x = packedRect.x;
			item.value->packedRect.y = packedRect.y;
			item.value->packedRect.width = packedRect.width;
			item.value->packedRect.height = packedRect.height;
		}

		break;
	}
	case ImageAtlasPackPolicy::MaxRects:
	{
		MaxRectsBinPack maxRectsBinPack;

		maxRectsBinPack.Init(atlasWidth, atlasHeight);

		for (auto item : items)
		{
			packedRect = maxRectsBinPack.Insert(
				item.value->width + border2,
				item.value->height + border2,
				MaxRectsBinPack::FreeRectChoiceHeuristic::RectBestAreaFit);

			if (packedRect.width == 0 && packedRect.height == 0)
			{
				B_LOG_ERROR("No room for images into the atlas, make atlas bigger");
				return false;
			}

			item.value->packedRect.x = packedRect.x;
			item.value->packedRect.y = packedRect.y;
			item.value->packedRect.width = packedRect.width;
			item.value->packedRect.height = packedRect.height;
		}

		break;
	}
	case ImageAtlasPackPolicy::ShelfBin:
	{
		ShelfBinPack shelfBinPack;
		
		shelfBinPack.Init(atlasWidth, atlasHeight, useWasteMap);

		for (auto item : items)
		{
			packedRect = shelfBinPack.Insert(
				item.value->width + border2,
				item.value->height + border2,
				ShelfBinPack::ShelfChoiceHeuristic::ShelfBestAreaFit);

			if (packedRect.width == 0 && packedRect.height == 0)
			{
				B_LOG_ERROR("No room for images into the atlas, make atlas bigger");
				return false;
			}

			item.value->packedRect.x = packedRect.x;
			item.value->packedRect.y = packedRect.y;
			item.value->packedRect.width = packedRect.width;
			item.value->packedRect.height = packedRect.height;
		}

		break;
	}
	case ImageAtlasPackPolicy::Skyline:
	{
		SkylineBinPack skylineBinPack;

		skylineBinPack.Init(atlasWidth, atlasHeight, useWasteMap);

		for (auto item : items)
		{
			packedRect = skylineBinPack.Insert(
				item.value->width + border2,
				item.value->height + border2,
				SkylineBinPack::LevelChoiceHeuristic::LevelMinWasteFit);

			if (packedRect.width == 0 && packedRect.height == 0)
			{
				B_LOG_ERROR("No room for images into the atlas, make atlas bigger");
				return false;
			}

			item.value->packedRect.x = packedRect.x;
			item.value->packedRect.y = packedRect.y;
			item.value->packedRect.width = packedRect.width;
			item.value->packedRect.height = packedRect.height;
		}

		break;
	}
	default:
		break;
	}


	RectI atlasRect;

	atlasRect.set(0, 0, atlasWidth, atlasHeight);
	
	//TODO: resize the atlas automatically or give the option
	// to set max sizes
	for (auto item : items)
	{
		item.value->packedRect.x += itemBorderSize;
		item.value->packedRect.y += itemBorderSize;
		item.value->packedRect.width -= border2;
		item.value->packedRect.height -= border2;
		item.value->wasRotated = item.value->width != item.value->packedRect.width;

		B_LOG_DEBUG("Rect: "
			<< item.value->packedRect.x - itemBorderSize << " "
			<< item.value->packedRect.y - itemBorderSize << " "
			<< item.value->packedRect.width + border2 << " "
			<< item.value->packedRect.height + border2 << " "
			<< "(wasRotated = " << item.value->wasRotated << ")");

		if (!atlasRect.contains(item.value->packedRect))
		{
			B_LOG_ERROR("Atlas rect doesn't fit into atlas texture, make atlas size bigger");
			return false;
		}
	}

	// we have now the rects inside the atlas
	// create texture data and update the api texture
	u32 atlasImageDataSize = atlasWidth * atlasHeight;

	pixels.resize(atlasImageDataSize);

	if (!pixels.size())
	{
		return false;
	}

	if (fillColor.getRgba() == 0)
	{
		memset(pixels.data(), 0, atlasImageDataSize * 4);
	}
	else
	{
		u32 col = fillColor.getRgba();

		pixels.fill(0, pixels.size(), col);
	}

	for (auto item : items)
	{
		f32 correctionHeight = 0.0002f;
		f32 correctionHeight2 = 0.0001f;

		item.value->texCoords.set(
			(f32)item.value->packedRect.x / (f32)atlasWidth,
			(f32)item.value->packedRect.y / (f32)atlasHeight - correctionHeight,
			(f32)item.value->packedRect.width / (f32)atlasWidth,
			(f32)item.value->packedRect.height / (f32)atlasHeight + correctionHeight2);

		// copy item image to the atlas image buffer
		if (item.value->wasRotated)
		{
			for (u32 y = 0; y < item.value->height; y++)
			{
				for (u32 x = 0; x < item.value->width; x++)
				{
					u32 destIndex =
						item.value->packedRect.x + y + (item.value->packedRect.y + x) * atlasWidth;
					u32 srcIndex = y * item.value->width + (item.value->width - 1) - x;
					pixels[destIndex] = ((u32*)item.value->imageData)[srcIndex];
				}
			}
		}
		else
		for (u32 y = 0; y < item.value->height; y++)
		{
			for (u32 x = 0; x < item.value->width; x++)
			{
				u32 destIndex =
					item.value->packedRect.x + x + (item.value->packedRect.y + y) * atlasWidth;
				u32 srcIndex = x + y * item.value->width;
				pixels[destIndex] = ((u32*)item.value->imageData)[srcIndex];
			}
		}
	}

	return true;
}

void ImageAtlas::clearItems()
{
	for (auto item : items)
	{
		B_ASSERT(item.value);
		
		if (item.value)
			delete [] item.value->imageData;
		
		delete item.value;
	}
	
	items.clear();
}

void ImageAtlas::clearPixels()
{
	pixels.clear();
}

}