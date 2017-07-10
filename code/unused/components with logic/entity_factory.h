#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "base/types.h"
#include "game/types.h"
#include "base/class_registry.h"

namespace engine
{
using namespace base;

class ENGINE_API EntityFactory : public ClassInfo
{
public:
	EntityFactory() {}
	virtual ~EntityFactory() {}
	virtual bool composeEntity(EntityId entity, const String& templateName) = 0;
};

}