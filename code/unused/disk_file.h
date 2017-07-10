#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/string.h"
#include "base/array.h"
#include "base/file.h"

namespace base
{
class DiskFile : public File
{
public:
	DiskFile();
	virtual ~DiskFile();

	u64 size() const;
	u64 position() const;
	bool open(const char* pFilename, u32 aModeFlags);
	bool close();
	bool read(void* pBuffer, u64 aLen);
	bool write(const void* pBuffer, u64 aLen);
	bool seek(FileSeekMode aMode, u64 aOffset);
	bool eof() const;

protected:
	FILE* m_pDiskFile;
};
}