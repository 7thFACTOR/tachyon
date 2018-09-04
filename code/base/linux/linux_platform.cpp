// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#ifdef _LINUX
#include "base/platform.h"
#include "base/assert.h"
#include "base/util.h"
#include "base/string.h"
#include "base/logger.h"
#include <sys/sysinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/dir.h>
#include <errno.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <utime.h>
#include <pthread.h>
#include <fnmatch.h>

namespace base
{
void showFatalMessageBox(const String& message)
{
	//TODO: make a window in X11
	B_LOG_ERROR("FATAL MSGBOX: " << message);
}

void showErrorMessageBox(const String& message)
{
	//TODO: make a window in X11
	B_LOG_ERROR("ERROR MSGBOX: " << message);
}

MessageBoxResult showMessageBox(MessageBoxFlags flags, const String& message)
{
	//TODO: make a window in X11
	B_LOG_INFO("MSGBOX: " << message);
	return MessageBoxResult::Unknown;
}

void copyTextToClipboard(const String& text)
{
	//TODO
}

String pasteTextFromClipboard()
{
	//TODO
}

bool fileExists(const String& filename)
{
	struct stat  buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}

bool isPathValid(const String& path)
{
	struct stat buffer;
	int ret = stat(path.c_str(), &buffer);
	bool isdir = S_ISDIR(buffer.st_mode);

	return ret == 0 && isdir;
}

bool deleteFile(const String& filename)
{
	unlink(filename.c_str());
	return true;
}

bool copyFile(const String& srcFilename, const String& destFilename, bool overwrite)
{
	String str;
	
	str << "sudo copy " << srcFilename << " " << destFilename;
	system(str.c_str());
	
	return true;
}

u64 computeFileSize(const String& filename)
{
	struct stat st;

	stat(filename.c_str(), &st);
	auto size = st.st_size;

	return size;
}

FileAttributeFlags getFileAttributes(const String& filename)
{
	struct stat st;

	stat(filename.c_str(), &st);
	FileAttributeFlags fa = FileAttributeFlags::None;

	fa |= S_ISREG(st.st_mode) ? FileAttributeFlags::Normal : FileAttributeFlags::None;

	return fa;
}

void setFileAttributes(const String& filename, FileAttributeFlags attrs)
{
	B_ASSERT_NOT_IMPLEMENTED;
}

bool getFileDateTime(const String& filename, DateTime* creationTime, DateTime* lastAccess, DateTime* lastWrite)
{
	struct stat st;

	stat(filename.c_str(), &st);

	auto getTimeFor = [](DateTime* time, time_t& stime)
	{
		if (time)
		{
			auto t = localtime(&stime);
			time->hour = t->tm_hour;
			time->minute = t->tm_min;
			time->second = t->tm_sec;
			time->day = t->tm_mday;
			time->month = t->tm_mon;
			time->year = t->tm_year;
		}
	};

	getTimeFor(creationTime, st.st_ctime);
	getTimeFor(lastAccess, st.st_atime);
	getTimeFor(lastWrite, st.st_mtime);

	return true;
}

bool setFileDateTime(
	const String& filename,
	DateTime* creationTime,
	DateTime* lastAccess,
	DateTime* lastWrite)
{
	struct stat st;
	struct utimbuf new_times;

	stat(filename.c_str(), &st);

	time_t atime;
	time_t mtime;

	//TODO:
	B_ASSERT_NOT_IMPLEMENTED;

	new_times.actime = atime;
	new_times.modtime = mtime;
	utime(filename.c_str(), &new_times);

	return true;
}

void deleteFolder(const String& path)
{
	B_ASSERT_NOT_IMPLEMENTED;
}

bool createFolder(const String& path)
{
	//TODO: maybe better
	system((String("mkdir -p ") + path).c_str());
	
	return true;
}

FindFileHandle findFirstFile(const String& path, const String& filenameMask, FoundFileInfo& fileInfo)
{
	B_ASSERT_NOT_IMPLEMENTED;
	return 0;
}

bool findNextFile(FindFileHandle findHandle, FoundFileInfo& fileInfo)
{
	B_ASSERT_NOT_IMPLEMENTED;
	return false;
}

String getCurrentWorkingPath()
{
	const int maxSize = 2048;
	char path[maxSize] = "";

	getcwd(path, maxSize);

	return path;
}

void setCurrentWorkingPath(const String& path)
{
	chdir(path.c_str());
}

String getApplicationPath()
{
	int length;
	char fullpath[MAXPATHLEN];

	/*
	* /proc/self is a symbolic link to the process-ID subdir of /proc, e.g.
	* /proc/4323 when the pid of the process of this program is 4323.
	* Inside /proc/<pid> there is a symbolic link to the executable that is
	* running as this <pid>.  This symbolic link is called "exe". So if we
	* read the path where the symlink /proc/self/exe points to we have the
	* full path of the executable.
	*/
	//TODO: readlink not defined in cygwin

	//length = readlink("/proc/self/exe", fullpath, sizeof(fullpath));

	/*
	* Catch some errors:
	*/
	if (length < 0)
	{
		perror("resolving symlink /proc/self/exe.");
		return "";
	}
	
	if (length >= sizeof(fullpath))
	{
		fprintf(stderr, "Path too long.\n");
		return "";
	}

	fullpath[length] = '\0';
	
	return fullpath;
}

String getApplicationFilename()
{
	String path = getApplicationPath();
	String name;
	char c;
	int i = path.length() - 1;

	while (i >= 0 && (c = path[i]) != '/')
	{
		name = String(c) + name;
	}

	return name;
}

LibraryHandle loadDynamicLibrary(const String& filename)
{
	return (LibraryHandle)dlopen(filename.c_str(), RTLD_NOW);
}

bool freeDynamicLibrary(LibraryHandle libHandle)
{
	return !dlclose((void*)libHandle);
}

void* findLibraryFunctionAddress(LibraryHandle libHandle, const String& funcName)
{
	if (!libHandle)
	{
		return 0;
	}

	return (void*)dlsym((void*)libHandle, funcName.c_str());
}

void setThreadName(ThreadHandle handle, const String& name)
{
	//TODO:
	return;
}

ThreadHandle createThread(ThreadFunction func, void* userData, ThreadPriority priority, u64 stackSize, const String& name)
{
	int err;
	pthread_t tid;

	err = pthread_create(&tid, NULL, func, userData);
	
	if (err != 0)
	{
		B_LOG_ERROR("Cant create thread: " << strerror(err));
	}

	//TODO: priority is not real value
	pthread_setschedprio(tid, (int)priority);

	return tid;
}

ThreadId getCurrentThreadId()
{
	return pthread_self();
}

ThreadHandle getCurrentThreadHandle()
{
	return pthread_self();
}

void setThreadPriority(ThreadHandle handle, ThreadPriority priority)
{
	//TODO:
}

void setThreadAffinity(ThreadHandle handle, u32 affinity)
{
	//TODO:
}

void suspendThread(ThreadHandle handle)
{
	//TODO
}

void resumeThread(ThreadHandle handle)
{
	//TODO
}

void exitThread(u32 exitCode)
{
	pthread_exit((void*)exitCode);
}

bool closeThread(ThreadHandle handle, u32 maxTimeMsec)
{
	void* val = 0;

	//TODO: use wait ?
	return pthread_join(handle, &val) == 0;
}

CriticalSectionHandle createCriticalSection()
{
	CriticalSectionHandle cs = new pthread_mutex_t;

	*cs = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

	return cs;
}

void deleteCriticalSection(CriticalSectionHandle handle)
{
	delete handle;
}

void enterCriticalSection(CriticalSectionHandle handle)
{
	pthread_mutex_lock(handle);
}

void leaveCriticalSection(CriticalSectionHandle handle)
{
	pthread_mutex_unlock(handle);
}

bool tryEnterCriticalSection(CriticalSectionHandle handle)
{
	return 0 == pthread_mutex_trylock(handle);
}

f64 getTimeMilliseconds()
{
	struct timespec now;

	if (clock_gettime(CLOCK_MONOTONIC, &now))
		return 0;

	//TODO: test this, looks odd
	return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}

f64 getTimeSeconds()
{
	struct timespec now;

	if (clock_gettime(CLOCK_MONOTONIC, &now))
		return 0;

	//TODO: test this, looks odd
	return now.tv_sec + now.tv_nsec / 1000000.0;
}

f64 getTimeMicroseconds()
{
	struct timespec now;

	if (clock_gettime(CLOCK_MONOTONIC, &now))
		return 0;

	//TODO: test this, looks odd
	return now.tv_sec * 1000000.0 + now.tv_nsec / 1000.0;
}

f64 getTimeNanoseconds()
{
	struct timespec now;

	if (clock_gettime(CLOCK_MONOTONIC, &now))
		return 0;

	//TODO: test this, looks odd
	return now.tv_sec * 1000000000.0 + now.tv_nsec;
}

void sleep(u32 timeMSec)
{
	usleep(timeMSec * 1000);
}

void yield()
{
	pthread_yield();
}

SystemInfo getSystemInfo()
{
	SystemInfo si;
	struct sysinfo info;

	sysinfo(&info);

	auto cpuCount = []()
	{
		int cpu_count = 0;
		DIR *sys_cpu_dir = opendir("/sys/devices/system/cpu");

		if (sys_cpu_dir == NULL)
		{
			int err = errno;
			return -1;
		}

		const struct dirent *cpu_dir;

		while ((cpu_dir = readdir(sys_cpu_dir)) != NULL)
		{
			if (fnmatch("cpu[0-9]*", cpu_dir->d_name, 0) != 0)
			{
				continue;
			}
			cpu_count++;
		}

		return cpu_count;
	};

	si.totalRAM = info.totalram;
	si.freeRAM = info.freeram;
	si.processorArchitecture = ProcessorArchitecture::Cpu64bit;//TODO
	si.pageSize = 0;//TODO
	si.physicalProcessorCount = cpuCount();//TODO
	si.logicalProcessorCount = cpuCount();//TODO
	si.processorCoreCount = cpuCount();//TODO
	si.level1CacheCount = 0;//TODO
	si.level2CacheCount = 0;//TODO
	si.level3CacheCount = 0;//TODO
	si.operatingSystemInfo << "Linux";//TODO

	return si;
}

DateTime getSystemDateTime()
{
	DateTime dt;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	dt.year = tm.tm_year;
	dt.day = tm.tm_mday;
	dt.month = tm.tm_mon;
	dt.hour = tm.tm_hour;
	dt.minute = tm.tm_min;
	dt.second = tm.tm_sec;
	dt.weekDay = tm.tm_wday;
	dt.millisecond = 0;

	return dt;
}

}
#endif