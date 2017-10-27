// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "asset_compiler.h"
#include "base/stdio_logger.h"
#include "base/logger.h"

#ifdef _WINDOWS
#include <tchar.h>
#include <windows.h>
#endif

using namespace base;

#if !defined(_CONSOLE) && defined(_WINDOWS)
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
#elif defined(_CONSOLE) && defined(_WINDOWS)
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	CommandLineArguments args;

#if defined(_WINDOWS)
	args.parse(__argc, __wargv);
#else
	args.parse(argc, argv);
#endif

	StdioLogger logger;
	base::getBaseLogger().linkChild(&logger);

	if (args.hasSwitch("version"))
	{
		B_LOG_INFO(AC_VERSION);
		return 0;
	}

	B_LOG_INFO("Asset Compiler v" << AC_VERSION << " (C) 7thFACTOR Entertainment");

	if (argc < 2)
	{
		B_LOG_INFO("Usage: asset_compiler <options> <file.config>");
		return 0;
	}

	ac::AssetCompiler assetCompiler;

	assetCompiler.compile(args);

	return 0;
}

