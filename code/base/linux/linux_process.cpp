// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#ifdef _LINUX
#include "base/process.h"
#include "base/logger.h"
#include "base/defines.h"
#include "base/array.h"
#include "base/string.h"
#include "base/util.h"
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

namespace base
{
Process::Process()
{}

Process::Process(const String& filename, const String& args, ProcessFlags flags)
{
	Array<String> argsArray;
	
	explodeString(args, argsArray, " ");
	char** argStrings = new char* [argsArray.size() + 1];
	
	for (size_t i = 0; i < argsArray.size(); i++)
	{
		argStrings[i] = (char*)argsArray[i].c_str();
	}
	
	argStrings[argsArray.size()] = nullptr;
	
	execv(filename.c_str(), argStrings);
	
	delete [] argStrings;
	
	B_LOG_DEBUG("Executed process: " << filename << " args: " << args);
}

}

#endif