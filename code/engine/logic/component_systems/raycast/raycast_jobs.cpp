// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "logic/component_systems/raycast/raycast_jobs.h"
#include "core/globals.h"
#include "logic/logic.h"
#include "logic/components/render/transform.h"
#include "base/task_scheduler.h"
#include "base/math/conversion.h"
#include "base/logger.h"
#include "core/globals.h"

namespace engine
{
using namespace base;

void RaycastJob::execute(Task& task)
{}

void RaycastJob::raycastWorld(const TransformComponent& oldXform)
{}

}