// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"

namespace base
{
class Stream;

class B_API Streamable
{
public:
	virtual void streamData(Stream& stream) {}
};

}