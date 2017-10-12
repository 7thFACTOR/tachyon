// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/string.h"

namespace base
{

enum class ProcessFlags
{
	None,
	Hidden = B_BIT(0),
	WaitToFinish = B_BIT(1)
};
B_ENUM_AS_FLAGS(ProcessFlags);

//TODO: input, output, error handlers, forced exit, async execution
class B_API Process
{
public:
	Process();
	Process(const String& filename, const String& args, ProcessFlags flags);
};

}