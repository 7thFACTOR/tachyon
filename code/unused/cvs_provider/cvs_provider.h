#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/thread.h"
#include "base/mutex.h"
#include "base/cmdline_arguments.h"

namespace cvs
{
using namespace base;

#define CVS_VERSION "1.0.0"

class CvsProvider
{
public:
	CvsProvider();
	virtual ~CvsProvider();

protected:
};

}