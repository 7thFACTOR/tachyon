// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "logic/component_updaters/raycast/raycast_component_updater.h"
#include "base/qsort.h"
#include "core/globals.h"
#include "base/task_scheduler.h"
#include "base/job.h"
#include "logic/logic.h"
#include "base/util.h"

namespace engine
{
static bool sortTraceRayObjectsByDistance(RayHitInfo* a, RayHitInfo* b)
{
	if (a->timeOnRay > b->timeOnRay)
	{
		return true;
	}

	return false;
}

RaycastComponentUpdater::RaycastComponentUpdater()
{
#ifdef B_USE_JOB_PROFILING
	JenkinsLookup3Hash64 hasher;
	String name = "RaycastComponentUpdater";

	hasher.add(name.c_str(), name.length());

	raycastJob.tag = hasher.getHash32();
#endif
}

RaycastComponentUpdater::~RaycastComponentUpdater()
{}

void RaycastComponentUpdater::update(f32 deltaTime)
{}

bool RaycastComponentUpdater::traceRay(
	const Ray& ray,
	Array<RayHitInfo>& hits,
	u32 maxIntersectedObjects,
	RaycastFlags flags,
	TraceRayAccuracyType accuracy)
{
	//TODO: use a oct-tree or a hash space to speed up things ?
	// maxIntersectionDepthObjects should be the current closest ones, not random objects in scene
	hits.clear();
	//traceRayOnParentObject(rRay, rHitInfos, aMaxIntersectionDepthObjects, aAccuracy, m_pRootNode);

	// sort by time, the first one will be the first intersected scene object by the ray
	B_QSORT(RayHitInfo, hits.data(), hits.size(), sortTraceRayObjectsByDistance);

	return hits.notEmpty();
}

}