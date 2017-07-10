#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/dictionary.h"
#include "base/defines.h"
#include "core/defines.h"
#include "logic/types.h"

namespace base
{
struct Task;
}

namespace engine
{
using namespace base;

struct Component;
class Logic;
class World;
class Entity;

typedef u64 ComponentUpdaterId;

class ComponentUpdater
{
public:
	enum UpdatePriority
	{
		First = 0,
		Early = 1000,
		Normal = 2000,
		Later = 3000,
		Last = 4000
	};

	ComponentUpdater() {}
	virtual ~ComponentUpdater() {}
	virtual ComponentUpdaterId getId() const = 0;
	virtual void update(f32 deltaTime) = 0;
	virtual void onComponentAdded(World& world, Entity& entity, Component* component) {}
	virtual void onComponentRemoved(World& world, Entity& entity, Component* component) {}
};

}