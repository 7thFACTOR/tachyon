#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/ray.h"
#include "logic/component_updater.h"
#include "logic/component_updaters/raycast/raycast_jobs.h"

namespace engine
{
struct Component;
class Game;
class World;

enum class TraceRayAccuracyType
{
	//! brute and fast test for ray intersection, depends on the world object (usually intersect with bounding box or sphere)
	Brute,
	//! high accuracy intersection will be done (per triangle)
	High
};

struct RayHitInfo
{
	Component* component = nullptr;
	//! the time T = {0..1}, on the ray that intersected the object
	f32 timeOnRay = 0.0f;
	//! point of intersection
	Vec3 pointIn;
	//! point of exiting the object
	Vec3 pointOut;
	//! the normal at intersection point
	Vec3 normalIn;
	//! the normal at exiting point
	Vec3 normalOut;

	RayHitInfo() {}
};

union RaycastFlags
{
	struct
	{
		u32 meshes : 1;
		u32 primitiveColliders : 1;
		u32 meshColliders : 1;
		u32 transparentMeshes : 1;
		u32 invisiblesAlso : 1;
	};

	static RaycastFlags collidersOnly() { RaycastFlags f; f.meshColliders = f.primitiveColliders = 1; return f; }
	static RaycastFlags allVisible() { RaycastFlags f; f.flags = ~0; f.invisiblesAlso = 0; return f; }
	static RaycastFlags all() { RaycastFlags f; f.flags = ~0; return f; }

	u32 flags = 0;
};

struct RaycastTaskInfo
{
	Ray ray;
	RaycastFlags flags;
};

class RaycastComponentUpdater : public ComponentUpdater
{
public:
	RaycastComponentUpdater();
	virtual ~RaycastComponentUpdater();
	ComponentUpdaterId getId() const override { return StdComponentUpdaterId_Raycast; }
	void update(f32 deltaTime) override;

protected:
	bool traceRay(
		const Ray& ray,
		Array<RayHitInfo>& hitInfos,
		u32 maxIntersectedObjects,
		RaycastFlags flags,
		TraceRayAccuracyType accuracy);

	RaycastJob raycastJob;
};

}