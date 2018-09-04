// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/string.h"
#include "base/array.h"
#include <time.h>

namespace base
{
class Logger;

#ifdef _WINDOWS
typedef u32 ThreadReturnType;
#elif defined(_LINUX)
typedef void* ThreadReturnType;
#endif

typedef ThreadReturnType(*ThreadFunction)(void* userData);
typedef void* FindFileHandle;

enum class ThreadPriority
{
	AboveNormal,
	BelowNormal,
	Highest,
	Idle,
	Lowest,
	Normal,
	TimeCritical,

	Count
};

enum class MessageBoxFlags
{
	None,
	Error = B_BIT(0),
	Info = B_BIT(1),
	Exclamation = B_BIT(2),
	Question = B_BIT(3),
	Ok = B_BIT(4),
	OkCancel = B_BIT(5),
	YesNo = B_BIT(6),
	YesNoCancel = B_BIT(7)
};
B_ENUM_AS_FLAGS(MessageBoxFlags);

enum class MessageBoxResult
{
	Unknown,
	Yes,
	No,
	Ok,
	Cancel
};

enum class FindFileFlags
{
	None,
	Directory = B_BIT(0),
	Hidden = B_BIT(1),
	Normal = B_BIT(2),
	ReadOnly = B_BIT(3),
	System = B_BIT(4)
};
B_ENUM_AS_FLAGS(FindFileFlags);

enum class FileAttributeFlags
{
	None,
	Temporary = B_BIT(0),
	Hidden = B_BIT(1),
	Normal = B_BIT(2),
	ReadOnly = B_BIT(3),
	Archive = B_BIT(4),
	System = B_BIT(5)
};
B_ENUM_AS_FLAGS(FileAttributeFlags);

enum class ProcessorArchitecture
{
	Unknown,
	Cpu32bit,
	Cpu64bit
};

enum class OsArchitecture
{
	Unknown,
	Os32bit,
	Os64bit
};

struct SystemInfo
{
	u64 freeRAM = 0;
	u64 totalRAM = 0;
	u64 pageSize = 0;
	u32 physicalProcessorCount = 0;
	u32 logicalProcessorCount = 0;
	u32 processorCoreCount = 0;
	String operatingSystemInfo;
	u32 level1CacheCount = 0, level2CacheCount = 0, level3CacheCount = 0;
	ProcessorArchitecture processorArchitecture = ProcessorArchitecture::Unknown;
	OsArchitecture osArchitecture = OsArchitecture::Unknown;
};

class B_API DateTime
{
public:
	u16 day, month, year, weekDay,
		hour, minute, second, millisecond;

	DateTime() {}
	DateTime(u16 newDay, u16 newMonth, u16 newYear, u16 newWeekDay,
		u16 newHour, u16 newMinute, u16 newSecond, u16 newMillisecond)
	{
		day = newDay;
		month = newMonth;
		year = newYear;
		weekDay = newWeekDay;
		hour = newHour;
		minute = newMinute;
		second = newSecond;
		millisecond = newMillisecond;
	}

	bool operator > (const DateTime& other) const;
	bool operator < (const DateTime& other) const;
	bool operator == (const DateTime& other) const;
	bool operator != (const DateTime& other) const;
	u32 toUnixTime() const;
	String toString() const;
	void fromUnixTime(u32 time);
#ifdef _DEBUG
	void debug() const;
#endif

protected:
	void toTM(tm& time) const;
};

struct FoundFileInfo
{
	FindFileFlags fileFlags = FindFileFlags::None;
	u64 fileSize = 0;
	String filename;
	DateTime creationTime, lastAccessTime, lastWriteTime;
};

B_EXPORT void showFatalMessageBox(const String& message);
B_EXPORT void showErrorMessageBox(const String& message);
B_EXPORT MessageBoxResult showMessageBox(MessageBoxFlags flags, const String& message);
B_EXPORT void copyTextToClipboard(const String& text);
B_EXPORT String pasteTextFromClipboard();
B_EXPORT bool fileExists(const String& filename);
B_EXPORT String getFilenameExtension(const String& filename);
B_EXPORT String changeFilenameExtension(const String& filename, const String& newExt);
B_EXPORT String getFilenameName(const String& filename);
B_EXPORT String getFilenameBasename(const String& filename);
B_EXPORT String getFilenameNoExtension(const String& filename);
B_EXPORT String getFilenamePath(const String& filename, bool keepEndSlash = false);
B_EXPORT String mergePathName(const String& path, const String& name);
B_EXPORT String mergePathBasenameExtension(const String& path, const String& basename, const String& extension);
B_EXPORT String mergePathPath(const String& path1, const String& path2);
B_EXPORT bool isPathValid(const String& path);
B_EXPORT String beautifyPath(const String& path, bool addSlashIfNone = false);
B_EXPORT String appendPathSlash(const String& path);
B_EXPORT bool deleteFile(const String& filename);
B_EXPORT bool copyFile(const String& srcFilename, const String& destFilename, bool overwrite = true);
B_EXPORT u64 computeFileSize(const String& filename);
B_EXPORT FileAttributeFlags getFileAttributes(const String& filename);
B_EXPORT void setFileAttributes(const String& filename, FileAttributeFlags attrs);
B_EXPORT bool getFileDateTime(const String& filename, DateTime* creationTime, DateTime* lastAccess = nullptr, DateTime* lastWrite = nullptr);
B_EXPORT bool setFileDateTime(const String& filename, DateTime* creationTime, DateTime* lastAccess = nullptr, DateTime* lastWrite = nullptr);
B_EXPORT void deleteFolder(const String& path);
B_EXPORT bool createFolder(const String& path);
B_EXPORT bool createPath(const String& path);
B_EXPORT FindFileHandle findFirstFile(const String& path, const String& filenameMask, FoundFileInfo& outFileInfo);
//! find the next file from findFirstFile(), returns false if none found
B_EXPORT bool findNextFile(FindFileHandle findHandle, FoundFileInfo& outFileInfo);
B_EXPORT bool scanFileSystem(const String& path, const String& filenameMask, Array<FoundFileInfo>& outEntries, bool recursive = true, bool addFolders = false);
//! \return the current working directory (can be influenced in your apps on win32
//! by open/save dialogs and SetCurrentDirectory(...))
B_EXPORT String getCurrentWorkingPath();

//! set the current working directory (can be influenced in your apps on win32
//! by open/save dialogs and SetCurrentDirectory(...))
B_EXPORT void setCurrentWorkingPath(const String& path);

//! \return the current application's path, the executable's path
//! that's using the engine (no file name included)
B_EXPORT String getApplicationPath();

//! \return the full application file path and name
B_EXPORT String getApplicationFilename();

B_EXPORT LibraryHandle loadDynamicLibrary(const String& filename);
B_EXPORT bool freeDynamicLibrary(LibraryHandle libHandle);
B_EXPORT void* findLibraryFunctionAddress(LibraryHandle libHandle, const String& funcName);
B_EXPORT ThreadHandle createThread(
	ThreadFunction func,
	void* userData,
	ThreadPriority priority = ThreadPriority::Normal,
	u64 stackSize = 0,
	const String& name = "BaseThread");

B_EXPORT ThreadId getCurrentThreadId();
B_EXPORT ThreadHandle getCurrentThreadHandle();
B_EXPORT void setThreadPriority(ThreadHandle handle, ThreadPriority priority);
B_EXPORT void setThreadAffinity(ThreadHandle handle, u32 affinity);
B_EXPORT void setThreadName(ThreadHandle handle, const String& threadName);
B_EXPORT void suspendThread(ThreadHandle handle);
B_EXPORT void resumeThread(ThreadHandle handle);
B_EXPORT void exitThread(u64 exitCode);
B_EXPORT bool closeThread(ThreadHandle handle, u32 maxWaitMsec = 0);
B_EXPORT CriticalSectionHandle createCriticalSection();
B_EXPORT void deleteCriticalSection(CriticalSectionHandle handle);
B_EXPORT void enterCriticalSection(CriticalSectionHandle handle);
B_EXPORT void leaveCriticalSection(CriticalSectionHandle handle);
B_EXPORT bool tryEnterCriticalSection(CriticalSectionHandle handle);

//! \return the elapsed milliseconds since the computer started
B_EXPORT u32 getTimeMilliseconds();
//! \return the elapsed precise milliseconds since the computer started
B_EXPORT f64 getTimeMillisecondsPrecise();

//! pause execution for timeMsec milliseconds
B_EXPORT void sleep(u32 timeMSec);
B_EXPORT void yield();
B_EXPORT SystemInfo getSystemInfo();
B_EXPORT DateTime getSystemDateTime();

}