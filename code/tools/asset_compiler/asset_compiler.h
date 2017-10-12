// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "common.h"
#include "asset_processor.h"
#include "base/string.h"
#include "base/array.h"
#include "base/thread.h"
#include "base/mutex.h"
#include "base/json.h"
#include "base/cmdline_arguments.h"
#include "core/types.h"
#include "project.h"
#include "asset_compiler_worker.h"

namespace ac
{
using namespace base;
using namespace engine;

struct AssetProcessor;

class AssetCompiler
{
public:
	AssetCompiler();
	virtual ~AssetCompiler();

	bool compile(const CommandLineArguments& args);

	void stealWorkFor(AssetCompilerWorker* worker);
protected:
	void compileAssets(Array<Asset*>& assets);

	CommandLineArguments args;
	bool threaded;
	u32 workerThreadCount = 0;
	Project project;
	Array<AssetProcessor*> availableProcessors;
	Mutex lockWorkerThreads;
	Array<AssetCompilerWorker> workerThreads;
};

}