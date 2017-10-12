// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "core/resources/animation_resource_repository.h"
#include "base/defines.h"
#include "base/assert.h"
#include "base/logger.h"

namespace engine
{
bool AnimationResourceRepository::load(Stream& stream, ResourceId resId)
{
	AnimationResource& anim = *resources[resId];

	B_ASSERT_NOT_IMPLEMENTED

	return true;
}

void AnimationResourceRepository::unload(ResourceId resId)
{
	AnimationResource& anim = *resources[resId];

	B_ASSERT_NOT_IMPLEMENTED
}

}