// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#ifdef _WINDOWS
#include "base/process.h"
#include "base/logger.h"
#include "base/defines.h"
#include <windows.h>

namespace base
{
Process::Process()
{}

Process::Process(const String& filename, const String& args, ProcessFlags flags)
{
	PROCESS_INFORMATION pinfo;
	STARTUPINFO sinfo;

	memset(&pinfo, 0, sizeof(pinfo));
	memset(&sinfo, 0, sizeof(sinfo));
	sinfo.cb = sizeof(sinfo);
	sinfo.wShowWindow = !!(flags & ProcessFlags::Hidden) ? SW_HIDE : SW_SHOWNORMAL;
	sinfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//TODO: why cant it work with app name and I have to give it as appname+args ??
	if (!CreateProcess(
		NULL,
		(LPSTR)(filename + " " + args).c_str(),
		NULL, NULL, FALSE,
		0, NULL, NULL,
		&sinfo, &pinfo))
	{
		B_LOG_ERROR("Cannot start process: " << filename << " args:" << args);
		return;
	}

	B_LOG_INFO("Started process: " << filename << " args:" << args);

	if (!!(flags & ProcessFlags::WaitToFinish))
	{
		WaitForSingleObject(pinfo.hProcess, INFINITE);
	}

	CloseHandle(pinfo.hProcess);
	CloseHandle(pinfo.hThread);
	B_LOG_DEBUG("Executed process: " << filename << " args: " << args);
}

}

#endif