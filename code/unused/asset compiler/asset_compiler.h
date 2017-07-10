#pragma once
#include "common.h"
#include "asset_processor.h"
#include "base/string.h"
#include "base/array.h"
#include "base/thread.h"
#include "base/rmutex.h"
#include "base/json.h"
#include "base/args_parser.h"
#include "core/types.h"
#include "project.h"
#include "asset_compiler_worker.h"

namespace ac
{
using namespace base;
using namespace engine;

class AssetProcessor;

class AssetCompiler
{
public:
	AssetCompiler();
	virtual ~AssetCompiler();

	bool compile(const ArgsParser& rArgs);

protected:
	ArgsParser m_args;
	bool m_bThreaded;
	u32 m_workerThreadCount;
	Project m_project;
	Array<AssetProcessor*> m_processors;
	Array<AssetCompilerWorker> m_workerThreads;
};

}