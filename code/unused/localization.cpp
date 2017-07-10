#include <string.h>
#include "base/localization.h"
#include "base/file.h"
#include "base/util.h"
#include "base/assert.h"
#include "base/logger.h"

namespace base
{
LocalizedText::LocalizedText(const char* pFile)
{
	if (pFile)
	{
		load(pFile);
	}
}

LocalizedText::~LocalizedText()
{
}

bool LocalizedText::load(const char* pFilename)
{
	DiskFile* pFile = files().open(pFilename, FileMode::BinaryRead);

	if (!pFile)
	{
		B_LOG_ERROR("Could not load language labels file: '%s'", pFilename);

		return false;
	}

	m_labels.clear();
	u32 fileSize = (u32)pFile->size();

	if (!fileSize)
	{
		delete pFile;
		return true;
	}

	// read 2 bytes, UNICODE header
	u16 hdr;
	*pFile >> hdr;
	WideChar line[4096], labelValue;
	String  labelName;

	fileSize -= 2;
	u32 charCount = fileSize / 2;
	//TODO: use pool
	WideChar* pData = new WideChar[charCount];
	B_ASSERT(pData);
	memset(pData, 0, fileSize);
	pFile->read(pData, fileSize);
	//TODO: read labels
	//for (u32 i = 0; i < charCount; ++i)
	//{
	//	if (pData[i] == L'\n')
	//	{
	//		u32 j = 0;

	//		labelName = "";
	//		labelValue = L"";

	//		while (line[j] != L'=' && j < 4096)
	//		{
	//			labelName += (char)line[j++];
	//		}

	//		if (line[j] == L'=')
	//		{
	//			++j;

	//			if (j < 4096)
	//			{
	//				while (line[j] != L'\n' && j < 4096)
	//				{
	//					labelValue += line[j++];
	//				}
	//			}
	//		}

	//		m_labels[nytro::trim(labelName)] = labelValue;
	//		wcscpy(line, L"");

	//		continue;
	//	}
	//	else if (pData[i] == L'\r')
	//	{
	//		continue;
	//	}

	//	line += pData[i];
	//}

	delete [] pData;
	delete pFile;

	return true;
}

const WideChar* LocalizedText::label(const char* pName)
{
	return m_labels[pName];
}
}