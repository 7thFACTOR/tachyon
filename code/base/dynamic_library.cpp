// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/dynamic_library.h"
#include "base/platform.h"
#include "base/logger.h"
#include "base/defines.h"

namespace base
{
DynamicLibrary::DynamicLibrary(const String& filename)
{
	handle = 0;
	dllFilename = "";

	if (!filename.isEmpty())
	{
		load(filename);
	}
}

DynamicLibrary::~DynamicLibrary()
{
	free();
}

const String& DynamicLibrary::getFilename() const
{
	return dllFilename;
}

LibraryHandle DynamicLibrary::getHandle() const
{
	return handle;
}

bool DynamicLibrary::load(const String& filename)
{
	dllFilename = filename;
	handle = loadDynamicLibrary(filename);

	return handle != 0;
}

bool DynamicLibrary::free()
{
	bool res = freeDynamicLibrary(handle);

	if (!res)
	{
		B_LOG_ERROR("Error when releasing dynamic library: '" << dllFilename << "', error code: " << res);

		return false;
	}

	return true;
}

void* DynamicLibrary::findFunctionAddress(const String& funcName) const
{
	return findLibraryFunctionAddress(handle, funcName);
}

}