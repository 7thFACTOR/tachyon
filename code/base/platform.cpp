// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <stdio.h>
#include <string.h>
#include "base/platform.h"
#include "base/assert.h"
#include "base/util.h"
#include "base/logger.h"

namespace base
{
bool DateTime::operator > (const DateTime& other) const
{
	if (year > other.year)
		return true;

	if (month > other.month)
		return true;

	if (day > other.day)
		return true;
	
	if (hour > other.hour)
		return true;

	if (minute > other.minute)
		return true;

	if (second > other.second)
		return true;

	if (millisecond > other.millisecond)
		return true;

	return false;
}

bool DateTime::operator < (const DateTime& other) const
{
	if (year < other.year)
		return true;

	if (month < other.month)
		return true;

	if (day < other.day)
		return true;

	if (hour < other.hour)
		return true;

	if (minute < other.minute)
		return true;

	if (second < other.second)
		return true;

	if (millisecond < other.millisecond)
		return true;

	return false;
}

bool DateTime::operator == (const DateTime& other) const
{
	return (year == other.year
		&& month == other.month
		&& day == other.day
		&& hour == other.hour
		&& minute == other.minute);
}

bool DateTime::operator != (const DateTime& other) const
{
	return (year != other.year
		|| month != other.month
		|| day != other.day
		|| hour != other.hour
		|| minute != other.minute);
}

void DateTime::toTM(tm& outTime) const
{
	memset(&outTime, 0, sizeof(outTime));
	outTime.tm_hour = hour;
	outTime.tm_min = minute;
	outTime.tm_sec = second;
	outTime.tm_mday = day;
	outTime.tm_mon = month - 1;
	outTime.tm_year = year - 1900;
	outTime.tm_wday = weekDay;
	outTime.tm_isdst = -1;
}

u32 DateTime::toUnixTime() const
{
	tm time;

	toTM(time);

	return mktime(&time);
}

String DateTime::toString() const
{
	return String() << hour << ":" << minute << ":" << second << ":" << millisecond << " " << day << "." << month << "." << year;
}

void DateTime::fromUnixTime(u32 time)
{
	time_t t = (time_t)time;
	tm* timeTm = localtime(&t);

	if (timeTm)
	{
		day = timeTm->tm_mday;
		month = timeTm->tm_mon;
		year = timeTm->tm_year;
		hour = timeTm->tm_hour;
		minute = timeTm->tm_min;
		second = timeTm->tm_sec;
		weekDay = timeTm->tm_wday;
		millisecond = 0;
	}
}

#ifdef _DEBUG
void DateTime::debug() const
{
	B_LOG_INFO("DateTime: "
		<< day << "." << month << "." << year << " "
		<< hour << ":" << minute << ":" << second << ":" << millisecond
		<< " weekDay: " << weekDay << " Epoch: " << toUnixTime());
}
#endif

String getFilenameExtension(const String& filename)
{
	int i = filename.length() - 1;
	String ext = "";

	if (filename.isEmpty())
	{
		return ext;
	}

	int lastDot = -1;

	while (i >= 0)
	{
		if (filename[i] == '/'
			|| filename[i] == '\\')
		{
			break;
		}

		if (filename[i] == '.')
		{
			lastDot = i;
		}
		--i;
	}

	if (lastDot != -1)
	{
		ext = filename.subString(lastDot, filename.length() - lastDot);
	}

	return ext;
}

String changeFilenameExtension(const String& filename, const String& newExtension)
{
	String oldExt = getFilenameExtension(filename);
	String newFilename = filename;

	newFilename.replace(oldExt, newExtension);

	return newFilename;
}

bool appendFileExtension(String& filename, const String& extension)
{
	if (filename.find(extension))
		return false;

	filename += extension;

	return true;
}

String getFilenameName(const String& filename)
{
	i32 i = 0;
	i32 j = 0, k = 0;

	String name;

	if (filename.isEmpty())
		return name;

	u32 len = filename.length();

	for (i = 0; i < len; ++i)
	{
		if (filename[i] == '\\' || filename[i] == '/')
		{
			j = i + 1;
		}
	}

	for (i = j; i < len; ++i)
	{
		name += filename[i];
	}

	return name;
}

String getFilenameBasename(const String& filename)
{
	i32 i = 0;
	i32 j = 0, k = 0;

	String basename;

	if (filename.isEmpty())
	{
		return "";
	}

	u32 len = filename.length();

	// find the beginning of the file name
	for (i = 0; i < len; ++i)
	{
		if (filename[i] == '\\' || filename[i] == '/')
		{
			j = i + 1;
		}
	}

	// store only the file name
	for (i = j; i < len; ++i)
	{
		basename += filename[i];
	}

	i = 0;

	// eliminate the extension
	while (i < basename.length() && basename[i] != '.')
	{
		i++;
	}

	if (i < basename.length())
	{
		basename = basename.subString(0, i);
	}

	return basename;
}

String getFilenameNoExtension(const String& filename)
{
	String str = filename;

	str = changeFilenameExtension(str, "");

	return str;
}

String getFilenamePath(const String& filename, bool keepEndSlash)
{
	i32 i = 0;
	i32 j = 0, k = 0;
	String path;

	if (filename.isEmpty())
	{
		return "";
	}

	u32 len = filename.length();

	for (i = 0; i < len; ++i)
	{
		if (filename[i] == '\\' || filename[i] == '/')
		{
			j = i + 1;
		}
	}

	for (i = 0; i < j; ++i)
	{
		path += filename[i];
	}

	if (!path.isEmpty())
	{
		if (keepEndSlash)
		{
			if (path[path.length() - 1] != '/')
			{
				path += '/';
			}
		}
		else
		{
			if (path[path.length() - 1] == '/')
			{
				path = path.subString(0, path.length() - 1);
			}
		}
	}

	return path;
}

String mergePathName(const String& path, const String& name)
{
	String result;

	if (!path.isEmpty())
	{
		if (path[path.length() - 1] == '\\'
			|| path[path.length() - 1] == '/')
		{
			result << path << name;
		}
		else
		{
			result << path << "/" << name;
		}
	}
	else
	{
		result = name;
	}

	return result;
}

String mergePathBasenameExtension(
		const String& path,
		const String& basename,
		const String& extension)
{
	String result = mergePathName(path, basename);
	result += extension;

	return result;
}

String mergePathPath(const String& path1, const String& path2)
{
	String result;

	if (!path1.isEmpty())
	{
		if (path1[path1.length() - 1] == '\\'
			|| path1[path1.length() - 1] == '/')
		{
			result = path1 + path2;
		}
		else
		{
			result << path1 << "/" << path2;
		}
	}
	else if (!path2.isEmpty())
	{
		result = path2;
	}

	return result;
}

String beautifyPath(const String& path, bool addSlashIfNone)
{
	if (path.isEmpty())
		return "";

	String str = path;

	// Unix style for paths: /some/dir/file or /some/dir/
	str.replace("\\", "/");
	str.replace("//", "/");

	if (str.length() > 0)
	{
		if (str[str.length() - 1])
		{
			if (str[str.length() - 1] == '/'
				|| str[str.length() - 1] == '\\')
			{
				String str2 = str.subString(0, str.length() - 1);
				str = str2;
			}
		}
	}

	if (!str.isEmpty() && addSlashIfNone)
	{
		if (str[str.length() - 1] != '/')
		{
			str += '/';
		}
	}

	return str;
}

String appendPathSlash(const String& path)
{
	String newPath = path;

	if (path.isEmpty())
	{
		return "";
	}

	if (path[path.length() - 1] == '/')
	{
		return path;
	}

	newPath += '/';

	return newPath;
}

bool createPath(const String& path)
{
	Array<String> dirs;
	
	explodeString(path, dirs, "/");

	if (dirs.isEmpty())
	{
		return false;
	}

	String dir = dirs[0];

	for (size_t i = 1; i < dirs.size(); ++i)
	{
		dir += "/";
		dir += dirs[i];
		createFolder(dir);
	}

	return true;
}

void scanFileSystemRecursive(
	const String& path, const String& filenameMask,
	Array<FoundFileInfo>& outEntries, bool recursive, bool addFolders)
{
	FoundFileInfo fileInfo;
	FindFileHandle hFind;

	//B_LOG_INFO("Scanning folder: '" << path << "' for mask: '" << filenameMask << "'");

	if (!(hFind = findFirstFile(path, "*", fileInfo)))
	{
		return;
	}

	do
	{
		if (!!(fileInfo.fileFlags & FindFileFlags::Directory))
		{
			if (fileInfo.filename != "." 
				&& fileInfo.filename != "..")
			{
				if (addFolders)
					outEntries.append(fileInfo);

				String otherPath = mergePathPath(path, fileInfo.filename);
				otherPath = beautifyPath(otherPath);
				scanFileSystemRecursive(otherPath, filenameMask, outEntries, recursive, addFolders);
			}
		}
		else if (wildcardCompare(filenameMask, fileInfo.filename, false))
		{
			fileInfo.filename = mergePathName(path, fileInfo.filename);
			outEntries.append(fileInfo);
		}

	} while (findNextFile(hFind, fileInfo));
}

bool scanFileSystem(const String& path, const String& filenameMask, Array<FoundFileInfo>& outEntries, bool recursive, bool addFolders)
{
	scanFileSystemRecursive(path, filenameMask, outEntries, recursive, addFolders);
	return outEntries.notEmpty();
}

}