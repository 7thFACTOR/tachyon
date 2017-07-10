#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/string.h"

namespace base
{
class B_API DynamicLibrary
{
public:
	DynamicLibrary(const String& filename = "");
	virtual ~DynamicLibrary();

	const String& getFilename() const;
	LibraryHandle getHandle() const;
	bool load(const String& filename);
	bool free();
	void* findFunctionAddress(const String& funcName) const;

private:
	LibraryHandle handle = 0;
	String dllFilename;
};

}