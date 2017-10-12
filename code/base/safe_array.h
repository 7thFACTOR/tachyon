// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/assert.h"
#include "base/array.h"
#include "base/mutex.h"

namespace base
{
template <class Type>
class SafeArray : public Array<Type>
{
public:
	inline Mutex& getMutex() { return safeArrayLock; }

protected:
	Mutex safeArrayLock;
};

}