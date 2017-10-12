// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/logger.h"

namespace base
{
class B_API HtmlLogger : public Logger
{
public:
	HtmlLogger(const String& filename = "", bool append = false);
	virtual ~HtmlLogger();

	void write(u32 channelIndex, LogItemType type, const String& module, u32 codeLine, const String& message);
	bool open(const String& filename, bool append = false);
	bool close();
};

}