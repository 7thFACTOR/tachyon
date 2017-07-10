#include "asset_compiler.h"
#include "base/stdio_logger.h"
#include "base/logger.h"

using namespace base;

//TODO: make the AC also as a DLL to be used by editor so it can show messages?
// but probably not
int main(int argc, char* argv[])
{
	CommandLineArguments args(argc, argv);
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

