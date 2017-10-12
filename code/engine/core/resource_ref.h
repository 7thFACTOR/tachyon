// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"

namespace engine
{
template<typename ResourceClassType>
struct ResourceRef
{
	ResourceRef()
	{}
	
	ResourceRef(ResourceId id)
	{
		ptr = nullptr;
		resId = id;
	}

	ResourceClassType* getPtr()
	{
		if (ptr)
			return ptr;
	
		return ptr = getResources().load(resId);
	}

protected:
	ResourceId resId = nullResourceId;
	ResourceClassType* ptr = nullptr;
};

}