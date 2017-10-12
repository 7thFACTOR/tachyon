// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <assert.h>
#include "base/file.h"
#include "base/util.h"
#include "base/platform.h"
#include "base/logger.h"
#include "base/string.h"

namespace base
{
File::File()
	: fileHandle(0)
	, largeFile(false)
{}

File::File(const String& filename, FileOpenFlags openFlags)
{
	open(filename, openFlags);
}

File::~File()
{
	close();
}

u64 File::getSize() const
{
	if (fileHandle)
	{
		u64 curpos, length;

		curpos = ftell(fileHandle);
		fseek(fileHandle, 0L, SEEK_END);
		length = ftell(fileHandle);
		fseek(fileHandle, curpos, SEEK_SET);

		return length;
	}

	return 0;
}

u64 File::getPosition() const
{
	if (fileHandle)
	{
		return ftell(fileHandle);
	}

	B_ASSERT(!"File is closed, cannot ftell");

	return -1;
}

bool File::open(const String& openFilename, FileOpenFlags openFlags)
{
	close();

	if (openFilename.isEmpty())
		return false;

	filename = openFilename;
	flags = openFlags;

	String mode = "";

	if (!!(openFlags & FileOpenFlags::Read)
		&& !(openFlags & FileOpenFlags::Write)
		&& !(openFlags & FileOpenFlags::Create)
		&& !(openFlags & FileOpenFlags::Append))
		mode = "r";

	if (!(openFlags & FileOpenFlags::Read)
		&& !!(openFlags & FileOpenFlags::Write)
		&& !(openFlags & FileOpenFlags::Create)
		&& !(openFlags & FileOpenFlags::Append))
		mode = "w";

	if (!!(openFlags & FileOpenFlags::Read)
		&& !!(openFlags & FileOpenFlags::Write)
		&& !(openFlags & FileOpenFlags::Create)
		&& !(openFlags & FileOpenFlags::Append))
		mode = "r+";

	if ((!!(openFlags & FileOpenFlags::Read)
		|| !!(openFlags & FileOpenFlags::Write))
		&& !!(openFlags & FileOpenFlags::Create)
		&& !(openFlags & FileOpenFlags::Append))
		mode = "w+";

	if (!(openFlags & FileOpenFlags::Read)
		&& !(openFlags & FileOpenFlags::Write)
		&& !(openFlags & FileOpenFlags::Create)
		&& !!(openFlags & FileOpenFlags::Append))
		mode = "a";

	if (!(openFlags & FileOpenFlags::Read)
		&& !(openFlags & FileOpenFlags::Write)
		&& !!(openFlags & FileOpenFlags::Create)
		&& !!(openFlags & FileOpenFlags::Append))
		mode = "a+";

	if (!!(openFlags & FileOpenFlags::Binary))
		mode += "b";

	if (!!(openFlags & FileOpenFlags::Text))
		mode += "t";

	fileHandle = fopen(filename.c_str(), mode.c_str());

	if (!!(openFlags & FileOpenFlags::Read))
	{
		reading = true;
	}
	else
		reading = false;

	if (!fileHandle)
	{
		return false;
	}
	else
	{
		return true;
	}

	return false;
}

bool File::close()
{
	if (fileHandle)
	{
		fclose(fileHandle);
		fileHandle = 0;

		return true;
	}

	return false;
}

bool File::read(void* data, u64 dataSize)
{
	B_ASSERT(data);

	if (fileHandle && data && dataSize)
	{
		return fread(data, dataSize, 1, fileHandle) == dataSize;
	}
	return true;
}

bool File::write(const void* data, u64 dataSize)
{
	B_ASSERT(data);

	if (fileHandle && data && dataSize)
	{
		return fwrite(data, 1, dataSize, fileHandle) == dataSize;
	}

	return false;
}

bool File::seek(SeekMode mode, u64 offset)
{
	if (fileHandle)
	{
		fseek(fileHandle, offset, (int)mode);
		return feof(fileHandle) != 1;
	}

	return false;
}

bool File::isEndOfStream() const
{
	if (fileHandle)
	{
		return feof(fileHandle);
	}

	return true;
}

bool File::readString(String& str)
{
	if (!!(flags & FileOpenFlags::Text))
	{
		return readLine(str);
	}
	
	return Stream::readString(str);
}

bool File::writeString(const String& str)
{
	if (!!(flags & FileOpenFlags::Text))
	{
		return fputs(str.c_str(), fileHandle) >= 0;
	}

	return Stream::writeString(str);
}

bool File::readLine(String& line)
{
	if (!!(flags & FileOpenFlags::Text))
	{
		static const int maxCharsPerLine = 1024;
		char lineBuffer[maxCharsPerLine] = { 0 };

		bool result = fgets(lineBuffer, maxCharsPerLine, fileHandle) != 0;

		line = lineBuffer;

		return result;
	}

	return Stream::readLine(line);
}

}