#ifdef BASE_PLATFORM_IOS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base/platform.h"
#include "base/assert.h"
#include "base/util.h"
#include "base/logger.h"
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/time.h>

namespace base
{
void errorMessageBox(const char* pFmt, ...)
{
	String tmpstr;

	E_VARGS_FORMAT_TO_STRING(pFmt, tmpstr);
// 	ShowCursor(TRUE);
// 	MessageBox(nullptr, tmpstr.charPtr(), "Nytro Engine Error", MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_TOPMOST);
}

u32 messageBox(u32 aFlags, const char* pFmt, ...)
{
	String tmpstr;

	E_VARGS_FORMAT_TO_STRING(pFmt, tmpstr);

// 	u32 flagsWin32 = MB_TASKMODAL | MB_TOPMOST;
// 
// 	if (aFlags & eMsgBoxFlag_Error)
// 		flagsWin32 |= MB_ICONERROR;
// 
// 	if (aFlags & eMsgBoxFlag_Info)
// 		flagsWin32 |= MB_ICONINFORMATION;
// 
// 	if (aFlags & eMsgBoxFlag_Exclamation)
// 		flagsWin32 |= MB_ICONEXCLAMATION;
// 
// 	if (aFlags & eMsgBoxFlag_Question)
// 		flagsWin32 |= MB_ICONQUESTION;
// 
// 	if (aFlags & eMsgBoxFlag_Ok)
// 		flagsWin32 |= MB_OK;
// 
// 	if (aFlags & eMsgBoxFlag_OkCancel)
// 		flagsWin32 |= MB_OKCANCEL;
// 
// 	if (aFlags & eMsgBoxFlag_YesNo)
// 		flagsWin32 |= MB_YESNO;
// 
// 	if (aFlags & eMsgBoxFlag_YesNoCancel)
// 		flagsWin32 |= MB_YESNOCANCEL;
// 
// 	ShowCursor(TRUE);
// 
// 	u32 retval = MessageBox(nullptr, tmpstr.charPtr(), "Nytro Engine Message", flagsWin32);
// 
// 	if (retval == IDOK)
// 		retval = eMsgBoxFlag_Ok;
// 	else if (retval == IDCANCEL)
// 		retval = eMsgBoxResult_Cancel;
// 	else if (retval == IDYES)
// 		retval = eMsgBoxResult_Yes;
// 	else if (retval == IDNO)
// 		retval = eMsgBoxResult_No;

	return 0;
}

void copyTextToClipboard(const char* pText)
{
// 	if (!OpenClipboard(nullptr))
// 	{
// 		return;
// 	}
// 
// 	EmptyClipboard();
// 
// 	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, strlen(pText) + 1);
// 
// 	if (hglbCopy == nullptr)
// 	{
// 		CloseClipboard();
// 		return;
// 	}
// 
// 	LPSTR lptstrCopy = (LPSTR)GlobalLock(hglbCopy);
// 
// 	if (lptstrCopy)
// 		memcpy(lptstrCopy, pText, strlen(pText) + 1);
// 
// 	GlobalUnlock(hglbCopy);
// 	SetClipboardData(CF_TEXT, hglbCopy);
// 	CloseClipboard();
}

void pasteTextFromClipboard(String& rOutText)
{
	rOutText = "";

// 	if (!IsClipboardFormatAvailable(CF_TEXT))
// 		return;
// 
// 	if (!OpenClipboard(nullptr))
// 	{
// 		return;
// 	}
// 
// 	HGLOBAL hglb = GetClipboardData(CF_TEXT);
// 
// 	if (hglb != nullptr)
// 	{
// 		LPSTR lptstr = (LPSTR)GlobalLock(hglb);
// 
// 		if (lptstr != nullptr)
// 		{
// 			rOutText = lptstr;
// 			GlobalUnlock(hglb);
// 		}
// 	}
// 
// 	CloseClipboard();
}

bool fileExists(const char* pFile)
{
	//fstats();
	return true;// == PathFileExists(pFile);
}

bool isPathValid(const char* pPath)
{
	return true;// == PathIsDirectory(pPath);
}

bool deleteFile(const char* pFile)
{
	unlink(pFile);
	return true;
}

bool copyFile(const char* pSrcFile, const char* pDstFile, bool bOverwrite)
{
	String str;
	
	formatString(str, "copy %s %s", pSrcFile, pDstFile);
	system(str.charPtr());
	
	return true;
}

u32 fileAttributes(const char* pFile)
{
// 	DWORD attrs = GetFileAttributes(pFile);
// 
// 	return ((FILE_ATTRIBUTE_TEMPORARY & attrs) ? eFileAttr_Temporary : 0)
// 		| ((FILE_ATTRIBUTE_HIDDEN & attrs) ?  eFileAttr_Hidden : 0)
// 		| ((FILE_ATTRIBUTE_NORMAL & attrs) ?  eFileAttr_Normal : 0)
// 		| ((FILE_ATTRIBUTE_READONLY & attrs) ?  eFileAttr_Readonly : 0)
// 		| ((FILE_ATTRIBUTE_ARCHIVE & attrs) ?  eFileAttr_Archive : 0)
// 		| ((FILE_ATTRIBUTE_SYSTEM & attrs) ?  eFileAttr_System : 0);
// 
	return 0;
}

void setFileAttributes(const char* pFile, u32 aAttrs /*FileAttributes enum members, OR'ed*/)
{
// 	SetFileAttributes(
// 				pFile,
// 				((eFileAttr_Temporary & aAttrs) ?  FILE_ATTRIBUTE_TEMPORARY : 0)
// 				| ((eFileAttr_Hidden & aAttrs) ?  FILE_ATTRIBUTE_HIDDEN : 0)
// 				| ((eFileAttr_Normal & aAttrs) ?  FILE_ATTRIBUTE_NORMAL : 0)
// 				| ((eFileAttr_Readonly & aAttrs) ?  FILE_ATTRIBUTE_READONLY : 0)
// 				| ((eFileAttr_Archive & aAttrs) ?  FILE_ATTRIBUTE_ARCHIVE : 0)
// 				| ((eFileAttr_System & aAttrs) ?  FILE_ATTRIBUTE_SYSTEM : 0));
}

bool fileDateTime(const char* pFile, DateTime* pCreationTime, DateTime* pLastAccess, DateTime* pLastWrite)
{
// 	HANDLE hnd = CreateFile(pFile, GENERIC_READ, 0, 0, 0, 0, 0);
// 
// 	if (hnd)
// 	{
// 		FILETIME ftCreation, ftLastAccess, ftLastWrite;
// 
// 		if (GetFileTime(hnd, &ftCreation, &ftLastAccess, &ftLastWrite))
// 		{
// 			if (pCreationTime)
// 				*pCreationTime = getWinFileTimeToEngineTime(ftCreation);
// 
// 			if (pLastAccess)
// 				*pLastAccess = getWinFileTimeToEngineTime(ftLastAccess);
// 
// 			if (pLastWrite)
// 				*pLastWrite = getWinFileTimeToEngineTime(ftLastWrite);
// 
// 			return true;
// 		}
// 	}

	return false;
}

void deleteFolder(const char* pPath)
{
// 	SHFILEOPSTRUCT del;
// 
// 	ZeroMemory(&del, sizeof(del));
// 	del.hwnd = 0;
// 	del.wFunc = FO_DELETE;
// 	del.pFrom = pPath;
// 	del.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI | FOF_SILENT;
// 	SHFileOperation(&del);
}

bool createFolder(const char* pPath)
{
	mkdir(pPath, 0777);
	return true;
}

FindFileHandle findFirstFile(const char* pDir, const char* pMask, String& rFilename, FoundFileInfo* pFileInfo)
{
	FindFileHandle hFind = 0;
// 	WIN32_FIND_DATA findData;
// 	String file;
// 
// 	mergePathFile(pDir, pMask, file);
// 	hFind = FindFirstFile(file.charPtr(), &findData);
// 
// 	if (INVALID_HANDLE_VALUE == hFind)
// 		return false;
// 
// 	rFilename = findData.cFileName;
// 
// 	if (pFileInfo)
// 	{
// 		if (findData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
// 		{
// 			pFileInfo->fileFlags = eFindFileFlag_Normal;
// 		}
// 		else
// 		{
// 			pFileInfo->fileFlags =
// 				((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? eFindFileFlag_Directory : 0)
// 				| ((findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? eFindFileFlag_Hidden : 0)
// 				| ((findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? eFindFileFlag_Readonly : 0)
// 				| ((findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? eFindFileFlag_System : 0);
// 		}
// 
// 		pFileInfo->creationTime = getWinFileTimeToEngineTime(findData.ftCreationTime);
// 		pFileInfo->lastAccessTime = getWinFileTimeToEngineTime(findData.ftLastAccessTime);
// 		pFileInfo->lastWriteTime = getWinFileTimeToEngineTime(findData.ftLastWriteTime);
// 		pFileInfo->fileSize = (findData.nFileSizeHigh * (MAXDWORD + 1)) + findData.nFileSizeLow;
// 		pFileInfo->fileName = findData.cFileName;
// 	}
// 
	return hFind;
}

bool findNextFile(FindFileHandle hFind, String& rFilename, FoundFileInfo* pFileInfo)
{
// 	WIN32_FIND_DATA findData;
// 	i32 result = FindNextFile(hFind, &findData);
// 
// 	if (result)
// 	{
// 		rFilename = findData.cFileName;
// 
// 		if (pFileInfo)
// 		{
// 			if (findData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
// 			{
// 				pFileInfo->fileFlags = eFindFileFlag_Normal;
// 			}
// 			else
// 			{
// 				pFileInfo->fileFlags =
// 					((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? eFindFileFlag_Directory : 0)
// 					| ((findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? eFindFileFlag_Hidden : 0)
// 					| ((findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? eFindFileFlag_Readonly : 0)
// 					| ((findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? eFindFileFlag_System : 0);
// 			}
// 
// 			pFileInfo->creationTime = getWinFileTimeToEngineTime(findData.ftCreationTime);
// 			pFileInfo->lastAccessTime = getWinFileTimeToEngineTime(findData.ftLastAccessTime);
// 			pFileInfo->lastWriteTime = getWinFileTimeToEngineTime(findData.ftLastWriteTime);
// 			pFileInfo->fileSize = (findData.nFileSizeHigh * (MAXDWORD + 1)) + findData.nFileSizeLow;
// 			pFileInfo->fileName = findData.cFileName;
// 		}
// 
// 		return true;
// 	}
// 
// 	FindClose(hFind);

	return false;
}

String getCurrentWorkingPath()
{
	char path[255] = "";

	getcwd(path, 255);
	
	return path;
}

void setCurrentWorkingPath(const char* pPath)
{
	chdir(pPath);
}

String getApplicationPath()
{
	char szAppPath[255] = "";
	char szAppDirectory[255] = "";
/*
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	strncpy(szAppDirectory, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
	szAppDirectory[strlen(szAppDirectory)] = '\0';*/

	return szAppDirectory;
}

LibraryHandle loadDynamicLibrary(const char* pFile)
{
	return (LibraryHandle) dlopen(pFile, RTLD_NOW);
}

bool freeDynamicLibrary(LibraryHandle aLib)
{
	return !dlclose((void*) aLib);
}

void* findLibraryFunctionAddress(LibraryHandle aLib, const char* pFuncName)
{
	if (!aLib)
	{
		return 0;
	}

	return (void*) dlsym((void*)aLib, pFuncName);
}

// //
// // Usage: SetThreadName (-1, "MainThread");
// //
// typedef struct tagTHREADNAME_INFO
// {
// 	DWORD dwType; // must be 0x1000
// 	LPCSTR szName; // pointer to name (in user addr space)
// 	DWORD dwThreadID; // thread ID (-1=caller thread)
// 	DWORD dwFlags; // reserved for future use, must be zero
// } THREADNAME_INFO;

void setThreadName(u64 hThread, const char* pThreadName)
{
// 	THREADNAME_INFO info;
// 
// 	info.dwType = 0x1000;
// 	info.szName = pThreadName;
// 	info.dwThreadID = hThread;
// 	info.dwFlags = 0;
// 
// 	__try
// 	{
// 		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
// 	}
// 
// 	__except (EXCEPTION_CONTINUE_EXECUTION)
// 	{
// 	}
}

u64 createThread(
			ThreadFunction pFunc,
			void* pUserData,
			EThreadPriority aPriority,
			u32 aStackLen,
			const char* pName)
{
	u64 threadID;
	u32 level;

// 	switch (aPriority)
// 	{
// 	case eThreadPriority_AboveNormal:
// 		level = THREAD_PRIORITY_ABOVE_NORMAL;
// 		break;
// 	case eThreadPriority_BelowNormal:
// 		level = THREAD_PRIORITY_BELOW_NORMAL;
// 		break;
// 	case eThreadPriority_Highest:
// 		level = THREAD_PRIORITY_HIGHEST;
// 		break;
// 	case eThreadPriority_Idle:
// 		level = THREAD_PRIORITY_IDLE;
// 		break;
// 	case eThreadPriority_Lowest:
// 		level = THREAD_PRIORITY_LOWEST;
// 		break;
// 	case eThreadPriority_Normal:
// 		level = THREAD_PRIORITY_NORMAL;
// 		break;
// 	case eThreadPriority_TimeCritical:
// 		level = THREAD_PRIORITY_TIME_CRITICAL;
// 		break;
// 	};
// 
// 	u64 hThread = (u64)CreateThread(
// 								nullptr,
// 								aStackLen,
// 								(LPTHREAD_START_ROUTINE) pFunc,
// 								pUserData,
// 								CREATE_SUSPENDED,
// 								(DWORD*) &threadID);
// 
// 	if (hThread)
// 	{
// 		SetThreadPriority((HANDLE)hThread, level);
// 	}

	return 0;//hThread;
}

void setThreadPriority(u64 aThreadHandle, u32 aPriority)
{
	//SetThreadPriority((HANDLE)aThreadHandle, aPriority);
}

void suspendThread(u64 aThreadHandle)
{
	//if (-1 == SuspendThread((HANDLE)aThreadHandle))
	{
		E_LOG_ERROR("Cannot suspend thread handle 0x%x", aThreadHandle);
	}
}

void resumeThread(u64 aThreadHandle)
{
	//if (-1 == ResumeThread((HANDLE)aThreadHandle))
	{
		E_LOG_ERROR("Cannot resume thread handle 0x%x", aThreadHandle);
	}
}

void exitThread(u32 aExitCode)
{
	//ExitThread(aExitCode);
}

void killThread(u64 aThreadHandle, u32 aExitCode)
{
	//TerminateThread((HANDLE)aThreadHandle, aExitCode);
}

bool closeThread(u64 aThreadHandle, u32 aMaxTimeMsec)
{
	//ResumeThread((HANDLE)aThreadHandle);
// 	bool bExitOk = 
// 		WAIT_TIMEOUT != WaitForSingleObject(
// 							(HANDLE)aThreadHandle,
// 							!aMaxTimeMsec ? INFINITE : aMaxTimeMsec);
// 
// 	if (!bExitOk)
// 	{
// 		TerminateThread((HANDLE)aThreadHandle, -1000);
// 	}
// 
// 	CloseHandle((HANDLE)aThreadHandle);

	return false;//bExitOk;
}

u32 getThreadExitCode(u64 aThreadHandle)
{
	u32 exitCode = 0;

	//GetExitCodeThread((HANDLE) aThreadHandle, (LPDWORD)&exitCode);

	return exitCode;
}

u64 createCriticalSection()
{
	//LPCRITICAL_SECTION pCS = new CRITICAL_SECTION;

	return 0;//(u64)pCS;
}

void deleteCriticalSection(u64 aId)
{
	//DeleteCriticalSection((LPCRITICAL_SECTION)aId);
	//delete(LPCRITICAL_SECTION)aId;
}

bool enterCriticalSection(u64 aId)
{
	//EnterCriticalSection((LPCRITICAL_SECTION)aId);

	return true;
}

bool leaveCriticalSection(u64 aId)
{
	//LeaveCriticalSection((LPCRITICAL_SECTION)aId);

	return true;
}

bool tryEnterCriticalSection(u64 aId)
{
	return true;// == TryEnterCriticalSection((LPCRITICAL_SECTION)aId);
}

u32 getTickCount()
{
	struct timeval start, end;
	
	long mtime, seconds, useconds;
	
	gettimeofday(&start, nullptr);
	usleep(2000);
	gettimeofday(&end, nullptr);
	
	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

	return mtime;
}

double getAccurateTickCount()
{
	i64 currentTime = 0;
	i64 frequency = 1;

// 	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
// 	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
// 
// 	if(!frequency)
// 	{
// 		return 0.0f;
// 	}

	return (double)currentTime / (double)frequency;
}

void sleep(u32 aTimeMSec)
{
	usleep(aTimeMSec);
}

// // Helper function to count set bits in the processor mask.
// DWORD countSetBits(ULONG_PTR bitMask)
// {
// 	DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
// 	DWORD bitSetCount = 0;
// 	ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
// 	DWORD i;
// 
// 	for (i = 0; i <= LSHIFT; ++i)
// 	{
// 		bitSetCount += ((bitMask & bitTest) ? 1 : 0);
// 		bitTest /= 2;
// 	}
// 
// 	return bitSetCount;
// }

void getSystemInfo(SystemInfo& rSystemInfo)
{
// 	MEMORYSTATUS memstat;
// 	OSVERSIONINFO osver;
// 	SYSTEM_INFO sysInfo;
// 
// 	GlobalMemoryStatus(&memstat);
// 	GetSystemInfo(&sysInfo);
// 	rSystemInfo.totalRAM = memstat.dwTotalPhys;
// 	rSystemInfo.freeRAM = memstat.dwAvailPhys;
// 
// 	if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
// 		|| sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64
// 		|| sysInfo.dwProcessorType == PROCESSOR_INTEL_IA64)
// 	{
// 		rSystemInfo.processorArchitecture = SystemInfo::eProcessorArchitecture_64bit;
// 	}
// 	else
// 	{
// 		rSystemInfo.processorArchitecture = SystemInfo::eProcessorArchitecture_32bit;
// 	}
// 
// 	rSystemInfo.pageSize = sysInfo.dwPageSize;
// 	rSystemInfo.physicalProcessorCount = sysInfo.dwNumberOfProcessors;
// 
// 	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION();
// 	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = nullptr;
// 	DWORD returnLength = sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
// 	DWORD logicalProcessorCount = 0;
// 	DWORD numaNodeCount = 0;
// 	DWORD processorCoreCount = 0;
// 	DWORD processorL1CacheCount = 0;
// 	DWORD processorL2CacheCount = 0;
// 	DWORD processorL3CacheCount = 0;
// 	DWORD processorPackageCount = 0;
// 	DWORD byteOffset = 0;
// 	PCACHE_DESCRIPTOR Cache;
// 	BOOL done = FALSE;
// 
// 	while (!done)
// 	{
// 		DWORD rc = GetLogicalProcessorInformation(buffer, &returnLength);
// 
// 		if (FALSE == rc)
// 		{
// 			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
// 			{
// 				if (buffer)
// 				{
// 					free(buffer);
// 				}
// 
// 				buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
// 				N_ASSERT(buffer);
// 			}
// 			else
// 			{
// 				N_LOG_ERROR("GetLogicalProcessorInformation Error: %d", GetLastError());
// 				break;
// 			}
// 		}
// 		else
// 		{
// 			done = TRUE;
// 		}
// 	}
// 
// 	ptr = buffer;
// 
// 	if (ptr)
// 	{
// 		while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
// 		{
// 			switch (ptr->Relationship)
// 			{
// 			case RelationNumaNode:
// 				// Non-NUMA systems report a single record of this type.
// 				++numaNodeCount;
// 				break;
// 
// 			case RelationProcessorCore:
// 				++processorCoreCount;
// 
// 				// A hyper-threaded core supplies more than one logical processor.
// 				logicalProcessorCount += countSetBits(ptr->ProcessorMask);
// 				break;
// 
// 			case RelationCache:
// 				// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache.
// 				Cache = &ptr->Cache;
// 
// 				if (Cache->Level == 1)
// 				{
// 					++processorL1CacheCount;
// 				}
// 				else if (Cache->Level == 2)
// 				{
// 					++processorL2CacheCount;
// 				}
// 				else if (Cache->Level == 3)
// 				{
// 					++processorL3CacheCount;
// 				}
// 				break;
// 
// 			case RelationProcessorPackage:
// 				// Logical processors share a physical package.
// 				++processorPackageCount;
// 				break;
// 
// 			default:
// 				N_LOG_ERROR("Error: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.");
// 				break;
// 			}
// 			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
// 			++ptr;
// 		}
// 
// 		free(buffer);
// 	}
// 
// 	rSystemInfo.logicalProcessorCount = logicalProcessorCount;
// 	rSystemInfo.processorCoreCount = processorCoreCount;
// 	rSystemInfo.level1CacheCount = processorL1CacheCount;
// 	rSystemInfo.level2CacheCount = processorL2CacheCount;
// 	rSystemInfo.level3CacheCount = processorL3CacheCount;
// 
// 	memset(&osver, 0, sizeof(osver));
// 	osver.dwOSVersionInfoSize = sizeof(osver);
// 	GetVersionEx(&osver);

	formatString(
			rSystemInfo.operatingSystemInfo,
			"Linux");
}

void getSystemDateTime(DateTime& rDateTime)
{
// 	SYSTEMTIME st;
// 
// 	GetLocalTime(&st);
// 	rDateTime.year = st.wYear;
// 	rDateTime.day = st.wDay;
// 	rDateTime.month = st.wMonth;
// 	rDateTime.hour = st.wHour;
// 	rDateTime.minute = st.wMinute;
// 	rDateTime.second = st.wSecond;
// 	rDateTime.weekDay = st.wDayOfWeek;
// 	rDateTime.msecond = st.wMilliseconds;
}
void handleOsMessages(bool& bStopped)
{
}
}
#endif