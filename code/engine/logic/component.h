// Copyright (C) 2017 7thFACTOR Software, All rights reserved
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

#define DEFINE_COMPONENT(type, className) \
	static const ComponentTypeId typeId = type; \
    virtual size_t getSize() const { return sizeof(className); }

struct Component : public PropertyHolder, public Streamable
{
    DEFINE_COMPONENT(StdComponentTypeIds::StdComponentTypeId_Unknown, Component)
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