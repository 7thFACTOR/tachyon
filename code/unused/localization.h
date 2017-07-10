#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/string.h"
#include "base/map.h"

namespace base
{
//! This class will load language labels from a file (plain Unicode, fixed 16bit per character, no headers)\n
//! The format of the file is : labelName = Label Value ... etc. (separated by CRLF)
class B_API LocalizedText
{
public:
	LocalizedText(const char* pFile = nullptr);
	virtual ~LocalizedText();

	bool load(const char* pFile);
	const WideChar* label(const char* pName);

protected:
	Map<String, WideChar*> m_labels;
};
}