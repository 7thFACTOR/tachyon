#include "graphics/video_adapter.h"

namespace engine
{
VideoMode* VideoAdapter::findVideoMode(u32 width, u32 height, u32 bpp, u32 freq) const
{
	i32 lastHz = 0;
	VideoMode* lastGoodVideoMode = nullptr;

	for (auto& videoMode : videoModes)
	{
		if (videoMode.width == width
			&& videoMode.height == height
			&& (bpp ? videoMode.bitsPerPixel == bpp : true))
		{
			if (freq != 0 && videoMode.frequency == freq)
			{
				return &videoMode;
			}
			else if (videoMode.frequency >= lastHz)
			{
				lastGoodVideoMode = &videoMode;
				lastHz = videoMode.frequency;
			}
		}
	}

	return lastGoodVideoMode;
}

}