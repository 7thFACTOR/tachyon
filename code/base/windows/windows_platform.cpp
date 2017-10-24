// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#ifdef _WINDOWS
#include "base/platform.h"
#include "base/assert.h"
#include "base/util.h"
#include "base/string.h"
#include "base/logger.h"
#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>

namespace base
{
DateTime winFileTimeToEngineTime(const FILETIME& fileTime)
{
	DateTime dateTime;
	SYSTEMTIME systime1 = { 0 };
	SYSTEMTIME systime = { 0 };

	FileTimeToSystemTime(&fileTime, &systime1);
	SystemTimeToTzSpecificLocalTime(NULL, &systime1, &systime);
	dateTime.year = systime.wYear;
	dateTime.day = systime.wDay;
	dateTime.month = systime.wMonth;
	dateTime.hour = systime.wHour;
	dateTime.minute = systime.wMinute;
	dateTime.second = systime.wSecond;
	dateTime.weekDay = systime.wDayOfWeek;
	dateTime.millisecond = systime.wMilliseconds;

	return dateTime;
}

FILETIME engineTimeFromWinFileTime(const DateTime& fileTime)
{
	SYSTEMTIME dateTime = { 0 }, systime = { 0 };
	FILETIME ftime = { 0 };

	dateTime.wYear = fileTime.year;
	dateTime.wDay = fileTime.day;
	dateTime.wMonth = fileTime.month;
	dateTime.wHour = fileTime.hour;
	dateTime.wMinute = fileTime.minute;
	dateTime.wSecond = fileTime.second;
	dateTime.wDayOfWeek = fileTime.weekDay;
	dateTime.wMilliseconds = fileTime.millisecond;
	TzSpecificLocalTimeToSystemTime(NULL, &dateTime, &systime);
	SystemTimeToFileTime(&systime, &ftime);

	return ftime;
}

void showFatalMessageBox(const String& message)
{
	ShowCursor(TRUE);

	MessageBoxW(0, stringToUtf16(message).data(), L"Fatal Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
}

void showErrorMessageBox(const String& message)
{
	ShowCursor(TRUE);

	MessageBoxW(nullptr, stringToUtf16(message).data(), L"Engine Error", MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_TOPMOST);
}

MessageBoxResult showMessageBox(MessageBoxFlags flags, const String& message)
{
	u32 flagsWin32 = MB_TASKMODAL | MB_TOPMOST;

	if (!!(flags & MessageBoxFlags::Error))
		flagsWin32 |= MB_ICONERROR;

	if (!!(flags & MessageBoxFlags::Info))
		flagsWin32 |= MB_ICONINFORMATION;

	if (!!(flags & MessageBoxFlags::Exclamation))
		flagsWin32 |= MB_ICONEXCLAMATION;

	if (!!(flags & MessageBoxFlags::Question))
		flagsWin32 |= MB_ICONQUESTION;

	if (!!(flags & MessageBoxFlags::Ok))
		flagsWin32 |= MB_OK;

	if (!!(flags & MessageBoxFlags::OkCancel))
		flagsWin32 |= MB_OKCANCEL;

	if (!!(flags & MessageBoxFlags::YesNo))
		flagsWin32 |= MB_YESNO;

	if (!!(flags & MessageBoxFlags::YesNoCancel))
		flagsWin32 |= MB_YESNOCANCEL;

	ShowCursor(TRUE);

	int retval = MessageBoxW(0, stringToUtf16(message).data(), L"Message", flagsWin32);

	if (retval == IDOK)
		return MessageBoxResult::Ok;
	else if (retval == IDCANCEL)
		return MessageBoxResult::Cancel;
	else if (retval == IDYES)
		return MessageBoxResult::Yes;
	else if (retval == IDNO)
		return MessageBoxResult::No;

	return MessageBoxResult::Unknown;
}

void copyTextToClipboard(const String& text)
{
	if (!OpenClipboard(0))
	{
		return;
	}

	EmptyClipboard();

	Utf16CodeUnitArray wtext;

	stringToUtf16(text, wtext);

	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, wtext.size() * sizeof(wchar_t) + 2);

	if (hglbCopy == nullptr)
	{
		CloseClipboard();
		return;
	}

	LPWSTR lptstrCopy = (LPWSTR)GlobalLock(hglbCopy);

	if (lptstrCopy)
	{
		memcpy(lptstrCopy, wtext.data(), wtext.size() * sizeof(wchar_t) + 2);
	}

	GlobalUnlock(hglbCopy);
	SetClipboardData(CF_UNICODETEXT, hglbCopy);
	CloseClipboard();
}

String pasteTextFromClipboard()
{
	String text = "";

	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
		return "";

	if (!OpenClipboard(0))
	{
		return "";
	}

	HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);

	if (hglb != nullptr)
	{
		LPWSTR lptstr = (LPWSTR)GlobalLock(hglb);

		if (lptstr != nullptr)
		{
			text = stringFromUtf16(lptstr);
			GlobalUnlock(hglb);
		}
	}

	CloseClipboard();

	return text;
}

bool fileExists(const String& filename)
{
	return FALSE != PathFileExistsW(stringToUtf16(filename).data());
}

bool isPathValid(const String& path)
{
	return FALSE != PathIsDirectoryW(stringToUtf16(path).data());
}

bool deleteFile(const String& filename)
{
	return FALSE != DeleteFileW(stringToUtf16(filename).data());
}

bool copyFile(const String& srcFilename, const String& destFilename, bool overwrite)
{
	return FALSE != CopyFileW(stringToUtf16(srcFilename).data(), stringToUtf16(destFilename).data(), !overwrite);
}

u64 computeFileSize(const String& filename)
{
	HANDLE hnd = CreateFileW(stringToUtf16(filename).data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	LARGE_INTEGER li;
	
	li.QuadPart = 0;
	GetFileSizeEx(hnd, &li);
	CloseHandle(hnd);

	return li.QuadPart;
}

FileAttributeFlags getFileAttributes(const String& filename)
{
	DWORD attrs = GetFileAttributesW(stringToUtf16(filename).data());
	FileAttributeFlags fa = FileAttributeFlags::None;

	fa |= (FILE_ATTRIBUTE_TEMPORARY & attrs) ? FileAttributeFlags::Temporary : FileAttributeFlags::None;
	fa |= (FILE_ATTRIBUTE_HIDDEN & attrs) ? FileAttributeFlags::Hidden : FileAttributeFlags::None;
	fa |= (FILE_ATTRIBUTE_NORMAL & attrs) ? FileAttributeFlags::Normal : FileAttributeFlags::None;
	fa |= (FILE_ATTRIBUTE_READONLY & attrs) ? FileAttributeFlags::ReadOnly : FileAttributeFlags::None;
	fa |= (FILE_ATTRIBUTE_ARCHIVE & attrs) ? FileAttributeFlags::Archive : FileAttributeFlags::None;
	fa |= (FILE_ATTRIBUTE_SYSTEM & attrs) ? FileAttributeFlags::System : FileAttributeFlags::None;

	return fa;
}

void setFileAttributes(const String& filename, FileAttributeFlags attrs)
{
	SetFileAttributesW(stringToUtf16(filename).data(),
		(!!(attrs & FileAttributeFlags::Temporary) ? FILE_ATTRIBUTE_TEMPORARY : 0)
		| (!!(attrs & FileAttributeFlags::Hidden) ?  FILE_ATTRIBUTE_HIDDEN : 0)
		| (!!(attrs & FileAttributeFlags::Normal) ? FILE_ATTRIBUTE_NORMAL : 0)
		| (!!(attrs & FileAttributeFlags::ReadOnly) ? FILE_ATTRIBUTE_READONLY : 0)
		| (!!(attrs & FileAttributeFlags::Archive) ? FILE_ATTRIBUTE_ARCHIVE : 0)
		| (!!(attrs & FileAttributeFlags::System) ? FILE_ATTRIBUTE_SYSTEM : 0));
}

bool getFileDateTime(const String& filename, DateTime* creationTime, DateTime* lastAccess, DateTime* lastWrite)
{
	HANDLE hnd = CreateFileW(stringToUtf16(filename).data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hnd != INVALID_HANDLE_VALUE)
	{
		FILETIME ftCreation, ftLastAccess, ftLastWrite;

		if (GetFileTime(hnd, &ftCreation, &ftLastAccess, &ftLastWrite))
		{
			if (creationTime)
				*creationTime = winFileTimeToEngineTime(ftCreation);

			if (lastAccess)
				*lastAccess = winFileTimeToEngineTime(ftLastAccess);

			if (lastWrite)
				*lastWrite = winFileTimeToEngineTime(ftLastWrite);

			CloseHandle(hnd);
			return true;
		}

		CloseHandle(hnd);
	}

	return false;
}

bool setFileDateTime(
	const String& filename,
	DateTime* creationTime,
	DateTime* lastAccess,
	DateTime* lastWrite)
{
	HANDLE hnd = CreateFileW(
		stringToUtf16(filename).data(),
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, 0);

	if (hnd != INVALID_HANDLE_VALUE)
	{
		FILETIME ftCreation, ftLastAccess, ftLastWrite;
		FILETIME *pftCreation = nullptr, *pftLastAccess = nullptr, *pftLastWrite = nullptr;

		if (creationTime)
		{
			ftCreation = engineTimeFromWinFileTime(*creationTime);
			pftCreation = &ftCreation;
		}

		if (lastAccess)
		{
			ftLastAccess = engineTimeFromWinFileTime(*lastAccess);
			pftLastAccess = &ftLastAccess;
		}

		if (lastWrite)
		{
			ftLastWrite = engineTimeFromWinFileTime(*lastWrite);
			pftLastWrite = &ftLastWrite;
		}

		BOOL ok = SetFileTime(hnd, pftCreation, pftLastAccess, pftLastWrite);
		B_ASSERT(ok);
		CloseHandle(hnd);

		return true;
	}
	
	return false;
}

void deleteFolder(const String& path)
{
	SHFILEOPSTRUCTW del;
	auto wpath = stringToUtf16(path);
	ZeroMemory(&del, sizeof(del));
	del.hwnd = 0;
	del.wFunc = FO_DELETE;
	del.pFrom = wpath.data();
	del.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI | FOF_SILENT;
	SHFileOperationW(&del);
}

bool createFolder(const String& path)
{
	return FALSE != CreateDirectoryW(stringToUtf16(path).data(), nullptr);
}

FindFileHandle findFirstFile(const String& path, const String& filenameMask, FoundFileInfo& fileInfo)
{
	Utf16CodeUnitArray wpath;
	Utf16CodeUnitArray wfilenameMask;

	stringToUtf16(path, wpath);
	stringToUtf16(filenameMask, wfilenameMask);

	FindFileHandle hFind = 0;
	WIN32_FIND_DATAW findData;
	String filename;

	filename = mergePathName(path, filenameMask);

	Utf16CodeUnitArray wfilename;

	stringToUtf16(filename, wfilename);

	hFind = FindFirstFileW(wfilename.data(), &findData);

	if (INVALID_HANDLE_VALUE == hFind)
		return nullptr;

	fileInfo.fileFlags = FindFileFlags::None;
	fileInfo.fileFlags |= (findData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) ? FindFileFlags::Normal : FindFileFlags::None;
	fileInfo.fileFlags |= (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? FindFileFlags::Directory : FindFileFlags::None;
	fileInfo.fileFlags |= (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? FindFileFlags::Hidden : FindFileFlags::None;
	fileInfo.fileFlags |= (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? FindFileFlags::ReadOnly : FindFileFlags::None;
	fileInfo.fileFlags |= (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? FindFileFlags::System : FindFileFlags::None;
	fileInfo.creationTime = winFileTimeToEngineTime(findData.ftCreationTime);
	fileInfo.lastAccessTime = winFileTimeToEngineTime(findData.ftLastAccessTime);
	fileInfo.lastWriteTime = winFileTimeToEngineTime(findData.ftLastWriteTime);
	fileInfo.fileSize = ((u64)findData.nFileSizeHigh * ((u64)MAXDWORD + 1)) + (u64)findData.nFileSizeLow;
	fileInfo.filename = stringFromUtf16(findData.cFileName);

	return hFind;
}

bool findNextFile(FindFileHandle findHandle, FoundFileInfo& fileInfo)
{
	WIN32_FIND_DATAW findData;
	i32 result = FindNextFileW(findHandle, &findData);

	if (result)
	{
		fileInfo.fileFlags = FindFileFlags::None;
		fileInfo.fileFlags |= (findData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) ? FindFileFlags::Normal : FindFileFlags::None;
		fileInfo.fileFlags |= (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? FindFileFlags::Directory : FindFileFlags::None;
		fileInfo.fileFlags |= (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? FindFileFlags::Hidden : FindFileFlags::None;
		fileInfo.fileFlags |= (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? FindFileFlags::ReadOnly : FindFileFlags::None;
		fileInfo.fileFlags |= (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? FindFileFlags::System : FindFileFlags::None;
		fileInfo.creationTime = winFileTimeToEngineTime(findData.ftCreationTime);
		fileInfo.lastAccessTime = winFileTimeToEngineTime(findData.ftLastAccessTime);
		fileInfo.lastWriteTime = winFileTimeToEngineTime(findData.ftLastWriteTime);
		fileInfo.fileSize = ((u64)findData.nFileSizeHigh * ((u64)MAXDWORD + 1)) + (u64)findData.nFileSizeLow;
		fileInfo.filename = stringFromUtf16(findData.cFileName);

		return true;
	}

	FindClose(findHandle);

	return false;
}

String getCurrentWorkingPath()
{
	WCHAR path[MAX_PATH] = { 0 };

	GetCurrentDirectoryW(MAX_PATH, path);

	return stringFromUtf16(path);
}

void setCurrentWorkingPath(const String& path)
{
	if (SetCurrentDirectoryW(stringToUtf16(path).data()))
	{
		B_LOG_ERROR("Cannot set current path to " << path);
	}
}

String getApplicationPath()
{
	WCHAR szAppPath[MAX_PATH] = L"";
	WCHAR szAppDirectory[MAX_PATH] = L"";

	::GetModuleFileNameW(0, szAppPath, sizeof(szAppPath) - 1);
	wcsncpy_s(szAppDirectory, MAX_PATH, szAppPath, wcsrchr(szAppPath, '\\') - szAppPath);
	szAppDirectory[wcslen(szAppDirectory)] = '\0';

	return stringFromUtf16(szAppDirectory);
}

String getApplicationFilename()
{
	WCHAR appPath[MAX_PATH] = { 0 };

	::GetModuleFileNameW(0, appPath, sizeof(appPath) - 1);

	return stringFromUtf16(appPath);
}

LibraryHandle loadDynamicLibrary(const String& filename)
{
	return (LibraryHandle) LoadLibraryW(stringToUtf16(filename).data());
}

bool freeDynamicLibrary(LibraryHandle libHandle)
{
	return FALSE != FreeLibrary((HINSTANCE)libHandle);
}

void* findLibraryFunctionAddress(LibraryHandle libHandle, const String& funcName)
{
	if (!libHandle)
	{
		return nullptr;
	}

	return (void*)GetProcAddress((HINSTANCE)libHandle, funcName.c_str());
}

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // must be 0x1000
	LPCSTR szName; // pointer to name (in user addr space)
	DWORD dwThreadID; // thread ID (-1=caller thread)
	DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;
#pragma pack(pop)

const DWORD MS_VC_EXCEPTION = 0x406D1388;

void setThreadName(ThreadHandle handle, const String& name)
{
#ifdef _DEBUG
	THREADNAME_INFO info;

	info.dwType = 0x1000;
	info.szName = name.c_str();
	info.dwThreadID = GetThreadId((HANDLE)handle);
	info.dwFlags = 0;

	__try { RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info); }
	__except (EXCEPTION_CONTINUE_EXECUTION) {}
#endif
}

ThreadHandle createThread(ThreadFunction func, void* userData, ThreadPriority priority, u64 stackSize, const String& name)
{
	u64 threadID;
	u32 level = 0;

	switch (priority)
	{
	case ThreadPriority::AboveNormal:
		level = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case ThreadPriority::BelowNormal:
		level = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case ThreadPriority::Highest:
		level = THREAD_PRIORITY_HIGHEST;
		break;
	case ThreadPriority::Idle:
		level = THREAD_PRIORITY_IDLE;
		break;
	case ThreadPriority::Lowest:
		level = THREAD_PRIORITY_LOWEST;
		break;
	case ThreadPriority::Normal:
		level = THREAD_PRIORITY_NORMAL;
		break;
	case ThreadPriority::TimeCritical:
		level = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	default:
		B_ASSERT(!"Invalid value for thread priority");
	};

	ThreadHandle hThread = (ThreadHandle)CreateThread(
		nullptr,
		stackSize,
		(LPTHREAD_START_ROUTINE) func,
		userData,
		CREATE_SUSPENDED,
		(DWORD*) &threadID);

	if (hThread)
	{
		SetThreadPriority((HANDLE)hThread, level);
	}

	setThreadName(hThread, name);

	return hThread;
}

ThreadId getCurrentThreadId()
{
	return (ThreadId)GetCurrentThreadId();
}

ThreadHandle getCurrentThreadHandle()
{
	return (ThreadHandle)GetCurrentThread();
}

void setThreadPriority(ThreadHandle handle, ThreadPriority priority)
{
	SetThreadPriority((HANDLE)handle, (int)priority);
}

void setThreadAffinity(ThreadHandle handle, u32 affinity)
{
	SetThreadAffinityMask((HANDLE)handle, affinity);
}

void suspendThread(ThreadHandle handle)
{
	if (-1 == SuspendThread((HANDLE)handle))
	{
		B_LOG_ERROR("Cannot suspend thread handle " << handle);
	}
}

void resumeThread(ThreadHandle handle)
{
	if (-1 == ResumeThread((HANDLE)handle))
	{
		B_LOG_ERROR("Cannot resume thread handle " << handle);
	}
}

void exitThread(u64 exitCode)
{
	ExitThread(exitCode);
}

bool closeThread(ThreadHandle handle, u32 maxTimeMsec)
{
	ResumeThread((HANDLE)handle);
	bool ok = WAIT_TIMEOUT != WaitForSingleObject((HANDLE)handle, !maxTimeMsec ? INFINITE : maxTimeMsec);

	if (!ok)
	{
		//TODO: this is the last resort, shouldn't happen in a normal release
		TerminateThread((HANDLE)handle, -1000);
	}

	CloseHandle((HANDLE)handle);

	return ok;
}

u64 getThreadExitCode(ThreadHandle handle)
{
	DWORD exitCode = 0;

	GetExitCodeThread((HANDLE)handle, (LPDWORD)&exitCode);

	return exitCode;
}

CriticalSectionHandle createCriticalSection()
{
	LPCRITICAL_SECTION cs = new CRITICAL_SECTION;
	InitializeCriticalSection(cs);

	return (CriticalSectionHandle)cs;
}

void deleteCriticalSection(CriticalSectionHandle handle)
{
	DeleteCriticalSection((LPCRITICAL_SECTION)handle);
	delete (LPCRITICAL_SECTION)handle;
}

void enterCriticalSection(CriticalSectionHandle handle)
{
	EnterCriticalSection((LPCRITICAL_SECTION)handle);
}

void leaveCriticalSection(CriticalSectionHandle handle)
{
	LeaveCriticalSection((LPCRITICAL_SECTION)handle);
}

bool tryEnterCriticalSection(CriticalSectionHandle handle)
{
	return FALSE != TryEnterCriticalSection((LPCRITICAL_SECTION)handle);
}

u32 getTimeMilliseconds()
{
	return timeGetTime();
}

f64 getTimeMicroseconds()
{
	LARGE_INTEGER currentTime;
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&currentTime);

	if (!frequency.QuadPart)
	{
		return 0.0f;
	}

	return (f64)currentTime.QuadPart * 1000.0f / (f64)frequency.QuadPart;
}

void sleep(u32 timeMSec)
{
	Sleep(timeMSec);
}

void yield()
{
	SwitchToThread();
}

static DWORD countSetBits(ULONG_PTR bitMask)
{
	DWORD leftShift = sizeof(ULONG_PTR) * 8 - 1;
	DWORD bitSetCount = 0;
	ULONG_PTR bitTest = (ULONG_PTR)1 << leftShift;
	DWORD i;

	for (i = 0; i <= leftShift; ++i)
	{
		bitSetCount += ((bitMask & bitTest) ? 1 : 0);
		bitTest /= 2;
	}

	return bitSetCount;
}

SystemInfo getSystemInfo()
{
	SystemInfo si;
	MEMORYSTATUSEX memstat;
	SYSTEM_INFO sysInfo;

	memstat.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memstat);
	GetSystemInfo(&sysInfo);
	si.totalRAM = memstat.ullTotalPhys;
	si.freeRAM = memstat.ullAvailPhys;

	if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
		|| sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64
		|| sysInfo.dwProcessorType == PROCESSOR_INTEL_IA64)
	{
		si.processorArchitecture = ProcessorArchitecture::Cpu64bit;
	}
	else
	{
		si.processorArchitecture = ProcessorArchitecture::Cpu32bit;
	}

	si.pageSize = sysInfo.dwPageSize;
	si.physicalProcessorCount = sysInfo.dwNumberOfProcessors;

	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = nullptr;
	DWORD returnLength = sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
	DWORD logicalProcessorCount = 0;
	DWORD numaNodeCount = 0;
	DWORD processorCoreCount = 0;
	DWORD processorL1CacheCount = 0;
	DWORD processorL2CacheCount = 0;
	DWORD processorL3CacheCount = 0;
	DWORD processorPackageCount = 0;
	DWORD byteOffset = 0;
	PCACHE_DESCRIPTOR cache;
	bool done = false;

	while (!done)
	{
		DWORD rc = GetLogicalProcessorInformation(buffer, &returnLength);

		if (FALSE == rc)
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer)
				{
					free(buffer);
				}

				buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
				B_ASSERT(buffer);
			}
			else
			{
				B_LOG_ERROR("GetLogicalProcessorInformation Error: " << (u32)GetLastError());
				break;
			}
		}
		else
		{
			done = true;
		}
	}

	ptr = buffer;

	if (ptr)
	{
		while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
		{
			switch (ptr->Relationship)
			{
			case RelationNumaNode:
				// Non-NUMA systems report a single record of this type.
				++numaNodeCount;
				break;

			case RelationProcessorCore:
				++processorCoreCount;

				// A hyper-threaded core supplies more than one logical processor.
				logicalProcessorCount += countSetBits(ptr->ProcessorMask);
				break;

			case RelationCache:
				// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache.
				cache = &ptr->Cache;

				if (cache->Level == 1)
				{
					++processorL1CacheCount;
				}
				else if (cache->Level == 2)
				{
					++processorL2CacheCount;
				}
				else if (cache->Level == 3)
				{
					++processorL3CacheCount;
				}
				break;

			case RelationProcessorPackage:
				// Logical processors share a physical package.
				++processorPackageCount;
				break;

			default:
				B_LOG_ERROR("Error: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.");
				break;
			}

			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			++ptr;
		}

		free(buffer);
	}

	si.logicalProcessorCount = logicalProcessorCount;
	si.processorCoreCount = processorCoreCount;
	si.level1CacheCount = processorL1CacheCount;
	si.level2CacheCount = processorL2CacheCount;
	si.level3CacheCount = processorL3CacheCount;
	si.operatingSystemInfo << "Windows";
	//TODO: GetVersionEx was deprecated, maybe use other API to get Windows version info

	return si;
}

DateTime getSystemDateTime()
{
	DateTime dt;
	SYSTEMTIME st;

	GetLocalTime(&st);
	dt.year = st.wYear;
	dt.day = st.wDay;
	dt.month = st.wMonth;
	dt.hour = st.wHour;
	dt.minute = st.wMinute;
	dt.second = st.wSecond;
	dt.weekDay = st.wDayOfWeek;
	dt.millisecond = st.wMilliseconds;

	return dt;
}

}

#endif