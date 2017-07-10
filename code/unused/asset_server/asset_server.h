#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/thread.h"
#include "base/mutex.h"
#include "base/cmdline_arguments.h"

namespace as
{
#define AS_VERSION "1.0.0"

class AssetServer
{
public:
	AssetServer();
	virtual ~AssetServer();

protected:
};

}