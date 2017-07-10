#include "game/systems/raycast/raycast_system.h"
#include "base/qsort.h"
#include "core/core.h"

namespace engine
{
B_REGISTER_CLASS(RaycastSystem);
	
bool sortTraceRayObjectsByDistance(RayHitInfo* pA, RayHitInfo* pB)
{
	if (pA->timeOnRay > pB->timeOnRay)
	{
		return true;
	}

	return false;
}

RaycastSystem::RaycastSystem()
{}

RaycastSystem::~RaycastSystem()
{}

bool RaycastSystem::traceRay(
	const Ray& ray,
	Array<RayHitInfo>& hits,
	u32 maxIntersectionDepthObjects,
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