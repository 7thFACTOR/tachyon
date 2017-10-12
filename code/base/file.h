// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/string.h"
#include "base/stream.h"
#include <stdio.h>

namespace base
{
enum class FileOpenFlags
{
	None,
	//! open the file in binary mode
	Binary = B_BIT(0),
	//! open the file in text mode
	Text = B_BIT(1),
	//! append data to file
	Append = B_BIT(2),
	//! write mode
	Write = B_BIT(3),
	//! read mode
	Read = B_BIT(4),
	//! create file if it doesn't exists
	Create = B_BIT(5),
	//! create or open as shared
	Shared = B_BIT(6),
	BinaryWrite = Binary | Write,
	BinaryRead = Binary | Read,
	BinaryReadWrite = Binary | Read | Write,
	TextWrite = Text | Write,
	TextRead = Text | Read,
	TextReadWrite = Text | Read | Write
};
B_ENUM_AS_FLAGS(FileOpenFlags);

class B_API File : public Stream
{
public:
	File();
	File(const String& filename, FileOpenFlags openFlags);
	virtual ~File();
	bool open(const String& filename, FileOpenFlags openFlags);
	bool close();
	u64 getSize() const override;
	u64 getPosition() const override;
	bool read(void* data, u64 dataSize) override;
	bool write(const void* data, u64 dataSize) override;
	bool seek(SeekMode mode, u64 offset) override;
	bool isEndOfStream() const override;
	FILE* getFileHandle() const { return fileHandle; }
	virtual bool readString(String& str) override;
	virtual bool writeString(const String& str) override;
	virtual bool readLine(String& line) override;

protected:
	FileOpenFlags flags = FileOpenFlags::None;
	bool largeFile = false;
	String filename;
	FILE* fileHandle = nullptr;
};

}