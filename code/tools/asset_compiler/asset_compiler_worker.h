#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/cmdline_arguments.h"
#include "base/thread.h"
#include "types.h"

namespace ac
{
class AssetCompilerWorker : public Thread
{
public:
	AssetCompilerWorker();
	~AssetCompilerWorker();

	void initialize(Project* project, u32 workerIndex, const CommandLineArguments& args);
	void addAsset(Asset* asset);
	size_t getAssetCount();
	void onRun();
	bool isBusy() const { return busy; }

public:
	Array<Asset*> assets;
	Mutex lockAssets;
	class AssetCompiler* compiler = nullptr;

protected:
	volatile bool busy = true;
	CommandLineArguments args;
	u32 workerIndex = 0;
	Array<AssetProcessor*> processors;
	Project* project = nullptr;
};

}