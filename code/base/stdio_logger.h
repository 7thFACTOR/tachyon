// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/logger.h"

#ifdef _WINDOWS
#include <windows.h>
#endif

namespace base
{
//! A stdio console app logger, writes what it receives with printf, useful for server or fast prototyping/debugging/testing
class B_API StdioLogger : public Logger
{
public:
	StdioLogger(const String& filename = "", bool append = false);
	~StdioLogger();

	void write(u32 channelIndex, LogItemType type, const String& module, u32 codeLine, const String& message) override;
	bool open(const String& filename, bool append = false) override;
	bool close() override;
protected:
#ifdef _WINDOWS
	HANDLE hConsole;
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD savedAttributes;
#endif
};

}