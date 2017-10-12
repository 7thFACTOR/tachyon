// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/defines.h"
#include "graphics/types.h"

namespace engine
{
using namespace base;

class E_API VideoAdapter
{
public:
	Array<VideoMode> videoModes;
	String name;

	//! \return nullptr if no video mode is found matching the arguments
	VideoMode* findVideoMode(u32 width, u32 height, u32 bpp = 0, u32 freq = 0) const;
};

}