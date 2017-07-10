#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "base/thread.h"
#include "base/thread_queue.h"

namespace engine
{
using namespace base;

class E_API RenderThread : public Thread
{
public:

	RenderThread();
	virtual ~RenderThread();
	virtual void onRun();

protected:
};
}