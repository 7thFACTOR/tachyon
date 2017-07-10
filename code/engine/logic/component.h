#pragma once
#include "base/defines.h"
#include "core/defines.h"
#include "logic/types.h"
#include "base/serialize.h"
#include "base/stream.h"
#include "core/property.h"
#include "base/streamable.h"

namespace engine
{
using namespace base;

class Entity;

struct Component : public PropertyHolder, public Streamable
{
	Component() {}
	virtual ~Component() {}

	virtual void streamData(Stream& stream) override
	{
		stream | used;
		stream | dirty;
	}

	bool used = false;
	bool dirty = false;
	Entity* entity = nullptr;
	ComponentTypeId type = StdComponentTypeId_Unknown;
};

}