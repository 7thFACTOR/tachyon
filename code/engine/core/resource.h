// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/platform.h"
#include "base/memory.h"
#include "core/defines.h"
#include "core/types.h"
#include "base/mutex.h"
#include "base/class_registry.h"
#include "base/util.h"
#include "base/logger.h"

namespace engine
{
using namespace base;
class Bundle;

struct Resource
{
	Bundle* bundle = nullptr;
	ResourceId resId = nullResourceId;
	ResourceType type = ResourceType::None;
	ResourceFlags resourceFlags = ResourceFlags::None;
	u64 dataSize = 0;
	u32 lastUsedTime = 0;
	bool dynamicResource = false;
};

E_EXPORT ResourceId toResourceId(const String& filename);
E_EXPORT ResourceId loadResource(const String& filename);

}