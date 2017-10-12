// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <stdio.h>
#include "base/stdio_logger.h"
#include "base/thread.h"
#include "base/util.h"

namespace base
{
StdioLogger::StdioLogger(const String& filename, bool append)
	: Logger()
{
	flags = LogFlags::Type;
	fileHandle = nullptr;
#ifdef _WINDOWS
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
}

StdioLogger::~StdioLogger()
{}

void StdioLogger::write(u32 channelIndex, LogItemType type, const String& module, u32 codeLine, const String& message)
{
	DateTime dt;
	String typeStr;
	String datetime, moduleStr, typeName, all, lineNumber, threadId, msgcolor;

	dt = getSystemDateTime();

	switch (type)
	{
	case LogItemType::Error:
		typeStr = " ERROR ";
		break;
	case LogItemType::Warning:
		typeStr = "WARNING";
		break;
	case LogItemType::Info:
		typeStr = "  INFO ";
		break;
	case LogItemType::Success:
		typeStr = "SUCCESS";
		break;
	case LogItemType::Debug:
		typeStr = " DEBUG ";
		break;
	case LogItemType::Header:
		typeStr = "============| ";
		break;
	default:
		typeStr = "";
		break;
	}

	if (!!(flags & LogFlags::DateTime))
	{
		datetime
			<< String::intDecimals(2)
			<< dt.hour << ":" << dt.minute << ":" << dt.second
			<< String::intDecimals(4)
			<< dt.millisecond;
	}

	if (!!(flags & LogFlags::Module))
	{
		moduleStr << " [" << module << "@" << codeLine << "] ";
	}

	if (!!(flags & LogFlags::Type))
	{
		typeName << "[" << typeStr << "]";
	}

	if (!!(flags & LogFlags::LineNumber))
	{
		lineNumber << "[" << String::intDecimals(6) << (lineCount++) << "]";
	}

	if (!!(flags & LogFlags::ThreadId))
	{
		threadId << "[" << (u64)getCurrentThreadId() << "]";
	}

	String indent;

	indent.resize(currentIndent * 4, ' ');

#ifdef _WINDOWS
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
	printf("%s %s", lineNumber.c_str(), datetime.c_str());

	DWORD attrs = 0;

	switch (type)
	{
	case LogItemType::Error:
		attrs = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case LogItemType::Warning:
		attrs = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case LogItemType::Info:
		attrs = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		break;
	case LogItemType::Success:
		attrs = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case LogItemType::Header:
		attrs = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	case LogItemType::User:
		attrs = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	case LogItemType::Debug:
		attrs = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	default:
		break;
	}

	SetConsoleTextAttribute(hConsole, attrs);
	printf("%s", typeName.c_str());
	printf( "%s %s ", indent.c_str(), message.c_str());
	
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED);
	printf("%s %s\n", moduleStr.c_str(), threadId.c_str());
	if (IsDebuggerPresent())
	{
		OutputDebugString((String() + lineNumber + " " + datetime + " " + typeName + " " + indent + " " + message + " " + moduleStr + threadId + "\n").c_str());
	}
#else
	printf("%s %s %s %s %s %s\n", lineNumber.c_str(), datetime.c_str(), typeName.c_str(),
		indent.c_str(), message.c_str(), moduleStr.c_str(), threadId.c_str());
#endif

	for (auto logger : childLoggers)
	{
		logger->write(channelIndex, type, module, codeLine, message);
	}
}

bool StdioLogger::open(const String& filename, bool append)
{
	close();
	return true;
}

bool StdioLogger::close()
{
	return true;
}

}