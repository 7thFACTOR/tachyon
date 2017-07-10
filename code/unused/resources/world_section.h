#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/rtti.h"
#include "base/stream.h"

namespace engine
{
using namespace base;

class WorldSectionResource : public base::Rtti
{
public:
	B_RUNTIME_CLASS;
	WorldSectionResource();
	virtual ~WorldSectionResource();
	bool load(Stream* pStream);
	bool unload();

protected:
};
}