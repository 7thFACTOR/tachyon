#include <stdio.h>
#include <string.h>
#include "base/disk_file.h"

namespace base
{
DiskFile::DiskFile()
	: File()
{
	m_pDiskFile = nullptr;
}

DiskFile::~DiskFile()
{
	close();
}

u64 DiskFile::size() const
{
	if (m_pDiskFile)
	{
		u64 curpos, length;

		curpos = ftell(m_pDiskFile);
		fseek(m_pDiskFile, 0L, SEEK_END);
		length = ftell(m_pDiskFile);
		fseek(m_pDiskFile, curpos, SEEK_SET);

		return length;
	}

	return 0;
}

u64 DiskFile::position() const
{
	if (m_pDiskFile)
	{
		return ftell(m_pDiskFile);
	}

	return 0;
}

bool DiskFile::open(const char* pFilename, u32 aModeFlags)
{
	close();

	if (!pFilename)
		return false;

	m_fileName = pFilename;

	String mode = "";

	if ((aModeFlags & FileMode::Read)
		&& !(aModeFlags & FileMode::Write)
		&& !(aModeFlags & FileMode::Create)
		&& !(aModeFlags & FileMode::Append))
		mode = "r";

	if (!(aModeFlags & FileMode::Read)
		&& (aModeFlags & FileMode::Write)
		&& !(aModeFlags & FileMode::Create)
		&& !(aModeFlags & FileMode::Append))
		mode = "w";

	if ((aModeFlags & FileMode::Read)
		&& (aModeFlags & FileMode::Write)
		&& !(aModeFlags & FileMode::Create)
		&& !(aModeFlags & FileMode::Append))
		mode = "r+";

	if (((aModeFlags & FileMode::Read)
		|| (aModeFlags & FileMode::Write))
		&& (aModeFlags & FileMode::Create)
		&& !(aModeFlags & FileMode::Append))
		mode = "w+";

	if (!(aModeFlags & FileMode::Read)
		&& !(aModeFlags & FileMode::Write)
		&& !(aModeFlags & FileMode::Create)
		&& (aModeFlags & FileMode::Append))
		mode = "a";

	if (!(aModeFlags & FileMode::Read)
		&& !(aModeFlags & FileMode::Write)
		&& (aModeFlags & FileMode::Create)
		&& (aModeFlags & FileMode::Append))
		mode = "a+";

	if (aModeFlags & FileMode::Binary)
		mode += "b";

	if (aModeFlags & FileMode::Text)
		mode += "t";

	m_pDiskFile = fopen(m_fileName.c_str(), mode.c_str());

	if (!m_pDiskFile)
	{
		return false;
	}
	else
	{
		return true;
	}

	return false;
}

bool DiskFile::close()
{
	if (m_pDiskFile)
	{
		fclose(m_pDiskFile);
		m_pDiskFile = 0;
	}

	return true;
}

bool DiskFile::read(void* pBuffer, u64 aLen)
{
	B_ASSERT(pBuffer);

	if (m_pDiskFile)
	{
		return fread(pBuffer, aLen, 1, m_pDiskFile) == aLen;
	}

	return false;
}

bool DiskFile::write(const void* pBuffer, u64 aLen)
{
	B_ASSERT(pBuffer);

	if (m_pDiskFile)
	{
		return fwrite(pBuffer, 1, aLen, m_pDiskFile) == aLen;
	}

	return false;
}

bool DiskFile::seek(FileSeekMode aMode, u64 aOffset)
{
	if (m_pDiskFile)
	{
		fseek(m_pDiskFile, aOffset, aMode);
		return feof(m_pDiskFile) != 1;
	}

	return false;
}

bool DiskFile::eof() const
{
	if (m_pDiskFile)
	{
		return feof(m_pDiskFile);
	}

	return true;
}
}