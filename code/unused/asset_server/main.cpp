#include "asset_server.h"
#include "base/stdio_logger.h"
#include "base/logger.h"

using namespace base;

int main(int argc, char* argv[])
{
	CommandLineArguments args(argc, argv);
	StdioLogger logger;
	getBaseLogger().open("asset_server.log");
	getBaseLogger().linkChild(&logger);

	if (args.hasSwitch("version"))
	{
		B_LOG_INFO("%s", AS_VERSION);
		return 0;
	}

	B_LOG_INFO("Asset Server v%s (C) 7thFACTOR Software", AS_VERSION);

	if (argc < 2)
	{
		B_LOG_INFO("Usage: asset_server <options> <file.config>");
		return 0;
	}

	as::AssetServer assetServer;

	//assetServer.start(args);

	return 0;
}

