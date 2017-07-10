#pragma once
#include "base/defines.h"
#include "base/math/vec3.h"
#include "base/math/ray.h"
#include "game/game_system.h"

namespace engine
{
struct Component;

//! This enum holds the scene object-ray intersection accuracy modes
enum class TraceRayAccuracyType
{
	//! brute and fast test for ray intersection, depends on the scene object (usually intersect with bounding box or sphere)
	Brute,
	//! high accuracy intersection will be done (per triangle)
	High
};

//! This class is used in conjunction with Scene::traceRay or SceneObject::traceRay method, to find the objects that are intersected by the given ray
struct RayHitInfo
{
	//! the intersected component
	Component* pComponent = nullptr;
	//! the time T = {0..1}, on the ray that intersected the object
	f32 timeOnRay = 0.0f;
	//! point of intersection
	Vec3 pointIn;
	//! point of exiting the scene object
	Vec3 pointOut;
	//! the normal at intersection point
	Vec3 normalIn;
	//! the normal at exiting point
	Vec3 normalOut;

	RayHitInfo() {}
};

class E_API RaycastSystem : public GameSystem
{
public:
	enum { Type = GameSystemType::Raycast };
	B_DECLARE_DERIVED_CLASS(RaycastSystem, GameSystem);
	RaycastSystem();
	virtual ~RaycastSystem();
	bool traceRay(
		const Ray& ray,
		Array<RayHitInfo>& hitInfos,
		u32 maxIntersectionDepthObjects,
		TraceRayAccuracyType accuracy);
};

}