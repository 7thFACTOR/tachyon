// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <stdio.h>
#include "base/logger.h"
#include "base/mutex.h"
#include "base/util.h"
#include "base/assert.h"

namespace base
{
Logger::Logger(const String& filename, bool append)
{
	flags |= LogFlags::Module | LogFlags::Type | LogFlags::LineNumber;
	fileHandle = nullptr;
	lineCount = 0;
	currentIndent = 0;

	if (filename.notEmpty())
	{
		open(filename, append);
	}
}

Logger::~Logger()
{}

void Logger::write(u32 channelIndex, LogItemType type, const String& module, u32 codeLine, const String& message)
{
	AutoLock<Mutex> lock(writeLock);

	if (fileHandle)
	{
		DateTime dt;
		String typeStr;

		dt = getSystemDateTime();

		switch (type)
		{
		case LogItemType::Error:
			typeStr = "  ERROR  ";
			break;
		case LogItemType::Warning:
			typeStr = " WARNING ";
			break;
		case LogItemType::Info:
			typeStr = "  INFO   ";
			break;
		case LogItemType::Success:
			typeStr = " SUCCESS ";
			break;
		case LogItemType::Debug:
			typeStr = "  DEBUG  ";
			break;
		case LogItemType::Header:
			typeStr = "";
			break;
		default:
			typeStr = "?";
			break;
		}

		String dateTime, moduleStr, typeName, all, lineNumber, threadId, indent;

		if (!!(flags & LogFlags::DateTime))
		{
			dateTime
				<< String::intDecimals(2)
				<< dt.hour << ":" << dt.minute << ":" << dt.second
				<< String::intDecimals(4)
				<< dt.millisecond
				<< String::intDecimals(2)
				<< dt.day << "/" << dt.month
				<< String::intDecimals(4)
				<< "/" << dt.year;
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

		indent.repeat(" ", currentIndent * 4);
		fwrite(lineNumber.c_str(), lineNumber.getByteSize(), 1, fileHandle);
		fwrite(dateTime.c_str(), dateTime.getByteSize(), 1, fileHandle);
		fwrite(typeName.c_str(), typeName.getByteSize(), 1, fileHandle);
		fwrite(indent.c_str(), indent.getByteSize(), 1, fileHandle);
		fwrite(message.c_str(), message.getByteSize(), 1, fileHandle);
		fwrite(moduleStr.c_str(), moduleStr.getByteSize(), 1, fileHandle);
		fwrite(threadId.c_str(), threadId.getByteSize(), 1, fileHandle);
		fwrite("\n", 1, 1, fileHandle);
		fflush(fileHandle);
	}

	++logCount[(u32)type];

	for (auto logger : childLoggers)
	{
		logger->write(channelIndex, type, module, codeLine, message);
	}
}

bool Logger::open(const String& filename, bool append)
{
	close();
	String openMode = append ? String("a+") : String("w");

	fileHandle = utf8fopen(filename, openMode);

	for (u32 i = 0; i < (u32)LogItemType::Count; ++i)
	{
		logCount[i] = 0;
	}

	return (fileHandle != nullptr);
}

bool Logger::isOpened() const
{
	return fileHandle != nullptr;
}

bool Logger::close()
{
	if (fileHandle)
	{
		write(
			LogChannelIndex::Main,
			LogItemType::Info,
			"Logger",
			__LINE__,
			String("Log reported:")
			<< logCount[(u32)LogItemType::Error] << " error(s),"
			<< logCount[(u32)LogItemType::Warning] << " warning(s),"
			<< logCount[(u32)LogItemType::Info] << " info(s),"
			<< logCount[(u32)LogItemType::Success] << " success(es),"
			<< logCount[(u32)LogItemType::Debug] << " debug(s)");
		fclose(fileHandle);
		fileHandle = nullptr;
	}
	
	return true;
}

void Logger::linkChild(Logger* childLogger)
{
	B_ASSERT(childLogger);
	childLoggers.append(childLogger);
}

void Logger::unlinkChild(Logger* childLogger)
{
	B_ASSERT(childLogger);
	Array<Logger*>::Iterator iter = childLoggers.find(childLogger);

	if (iter != childLoggers.end())
	{
		childLoggers.erase(iter);
	}
}

void Logger::increaseIndent()
{
	++currentIndent;
}

void Logger::decreaseIndent()
{
	--currentIndent;
}

#ifndef B_NO_LOGGER
Logger baseLogger;

Logger& getBaseLogger()
{
	return baseLogger;
}

void writeToBaseLog(u32 channelIndex, LogItemType type, const String& module, u32 codeLine, const String& message)
{
	if (!baseLogger.isOpened())
	{
		baseLogger.open(getApplicationFilename() + ".log");
	}

	// strip namespace from module name
	Array<String> strings;
	String stripped = module;

	explodeString(module, strings, "::");

	if (strings.size() >= 2)
	{
		stripped = strings[strings.size() - 2] + "::" + strings[strings.size() - 1];
	}

	baseLogger.write(channelIndex, type, stripped, codeLine, message);
}
#endif

}