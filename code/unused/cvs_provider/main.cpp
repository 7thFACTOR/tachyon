#include "cvs_provider.h"
#include "base/stdio_logger.h"
#include "base/logger.h"
#include "base/assert.h"
#include "base/defines.h"

using namespace base;

int main(int argc, char* argv[])
{
	CommandLineArguments args(argc, argv);
	StdioLogger logger;
	getBaseLogger().open("cvs_provider.log");
	getBaseLogger().linkChild(&logger);

	if (args.hasSwitch("version"))
	{
		B_LOG_INFO("%s", CVS_VERSION);
		return 0;
	}

	B_LOG_INFO("CVS Provider v%s (C) 7thFACTOR Software", CVS_VERSION);

	if (argc < 2)
	{
		B_LOG_INFO("Usage: cvs_provider <options>");
		return 0;
	}

	//nas::AssetServer assetServer;

	//assetServer.start(args);

	return 0;
}

