// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include <stdio.h>
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/mutex.h"
#include "base/assert.h"

namespace base
{
class Mutex;

enum class LogItemType
{
	Error,
	Warning,
	Info,
	Success,
	Debug,
	Header, //!< used to start a distinct section in the log
	User,

	Count
};

enum class LogFlags
{
	None,
	All = ~0,
	LineNumber = B_BIT(0),
	DateTime = B_BIT(1),
	Module = B_BIT(2),
	Type = B_BIT(3),
	ThreadId = B_BIT(4)
};
B_ENUM_AS_FLAGS(LogFlags);

namespace LogChannelIndex
{
	enum
	{
		Main = 0, //!< logs coming from main thread of the application
		Thread, //!< logs coming from various secondary threads of the application
		Base, //!< logs coming from the base sdk
		Engine, //!< logs coming from the engine sdk

		MaxCount = 64
	};
}

class B_API Logger
{
public:
	Logger(const String& filename = "", bool append = false);
	virtual ~Logger();
	virtual void write(u32 channelIndex, LogItemType type, const String& module, u32 codeLine, const String& message);
	virtual bool open(const String& filename, bool append = false);
	virtual bool close();
	virtual bool isOpened() const;
	//! link up another logger with this one, so you can have multiple logger
	//! chained up and logging into different destinations (disk,screen,etc.)
	virtual void linkChild(Logger* childLogger);
	virtual void unlinkChild(Logger* childLogger);
	virtual void increaseIndent();
	virtual void decreaseIndent();
	virtual void setChannelActive(u32 channelIndex, bool active) { channelInfo[channelIndex].active = active; }

public:
	LogFlags flags = LogFlags::All;

protected:
	struct B_API ChannelInfo
	{
		bool active = true;
	};
	
	Array<Logger*> childLoggers;
	FILE* fileHandle = nullptr;
	u32 logCount[(u32)LogItemType::Count];
	Mutex writeLock;
	ChannelInfo channelInfo[(u32)LogChannelIndex::MaxCount];
	u32 lineCount = 0;
	u32 currentIndent = 0;
};

struct LoggerAutoIndent
{
	Logger* logger = nullptr;

	LoggerAutoIndent(Logger* targetLogger)
	{
		B_ASSERT(targetLogger);
		targetLogger->increaseIndent();
		logger = targetLogger;
	}
	
	~LoggerAutoIndent()
	{
		logger->decreaseIndent();
	}
};

#ifndef B_EXCLUDE_LOGGER
B_EXPORT Logger& getBaseLogger();
B_EXPORT void writeToBaseLog(u32 channelIndex, LogItemType type, const String& module, u32 codeLine, const String& message);

#define B_LOG_WRITE(type, msg) base::writeToBaseLog(base::LogChannelIndex::Main, type, __FUNCTION__, __LINE__, String() << msg)
#define B_LOG_ERROR(msg) base::writeToBaseLog(base::LogChannelIndex::Main, base::LogItemType::Error, __FUNCTION__, __LINE__, String() << msg)
#define B_LOG_WARNING(msg) base::writeToBaseLog(base::LogChannelIndex::Main, base::LogItemType::Warning, __FUNCTION__, __LINE__, String() << msg)
#define B_LOG_INFO(msg) base::writeToBaseLog(base::LogChannelIndex::Main, base::LogItemType::Info, __FUNCTION__, __LINE__, String() << msg)
#define B_LOG_SUCCESS(msg) base::writeToBaseLog(base::LogChannelIndex::Main, base::LogItemType::Success, __FUNCTION__, __LINE__, String() << msg)
#define B_LOG_TEMP(msg) base::writeToBaseLog(base::LogChannelIndex::Main, base::LogItemType::Debug, __FUNCTION__, __LINE__, String() << msg)
#define B_LOG_HEADER(msg) base::writeToBaseLog(base::LogChannelIndex::Main, base::LogItemType::Header, __FUNCTION__, __LINE__, String() << msg)

#define B_LOG_INDENT base::getBaseLogger().increaseIndent();
#define B_LOG_UNINDENT base::getBaseLogger().decreaseIndent();
#define B_LOG_AUTO_INDENT base::LoggerAutoIndent autoLogIndent(&getBaseLogger());

#ifdef _DEBUG
	#define B_LOG_DEBUG(msg) base::writeToBaseLog(base::LogChannelIndex::Main, base::LogItemType::Debug, __FUNCTION__, __LINE__, String() << msg)
#else
	#define B_LOG_DEBUG(msg)
#endif

#else
#define B_LOG_WRITE(type, msg)
#define B_LOG_ERROR(msg)
#define B_LOG_WARNING(msg)
#define B_LOG_INFO(msg)
#define B_LOG_SUCCESS(msg)
#define B_LOG_INDENT
#define B_LOG_UNINDENT
#define B_LOG_AUTO_INDENT
#define B_LOG_TEMP(msg)
#define B_LOG_DEBUG(msg)
#define B_LOG_HEADER(msg)
#endif

}