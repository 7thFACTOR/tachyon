#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/dictionary.h"
#include "core/resource.h"
#include "base/math/vec3.h"
#include "base/math/color.h"

namespace engine
{
using namespace base;

enum class AnimationKeyType
{
	None,
	Float32,
	Float64,
	Int32,
	Uint32,
	Bool,
	Vec3,
	Color,
	
	Count
};

enum class AnimationLoopMode
{
	//! play once
	Once,
	//! play normally, then rewind and play again and repeat
	Repeat,
	//! play normally then inversed and repeat
	PingPong,
	
	Count
};

//! The track key interpolation mode
enum class AnimationInterpolation
{
	None,
	Linear,
	Bezier,
	TCB,
	
	Count
};

enum class AnimationCurveType
{
	TranslationX,
	TranslationY,
	TranslationZ,
	RotationX,
	RotationY,
	RotationZ,
	ScaleX,
	ScaleY,
	ScaleZ,
	Visibility,

	Count
};

struct AnimationKeyResource
{
	f32 time = 0;
	f32 easeIn = 0.0f;
	f32 easeOut = 0.0f;
	f32 tension = 0.5f;
	f32 continuity = 0.5f;
	f32 bias = 0.5f;
};

template <typename Type>
struct AnimationKeyResourceTpl : AnimationKeyResource
{
	Type value;
	Type left;
	Type right;
};

typedef AnimationKeyResourceTpl<f32> AnimationKeyResourceFloat32;
typedef AnimationKeyResourceTpl<f64> AnimationKeyResourceFloat64;
typedef AnimationKeyResourceTpl<i32> AnimationKeyResourceInt32;
typedef AnimationKeyResourceTpl<u32> AnimationKeyResourceUint32;
typedef AnimationKeyResourceTpl<bool> AnimationKeyResourceBool;
typedef AnimationKeyResourceTpl<Vec3> AnimationKeyResourceVec3;
typedef AnimationKeyResourceTpl<Color> AnimationKeyResourceColor;

struct AnimationTrackResource
{
	u32 id = 0;
	AnimationKeyType keyType = AnimationKeyType::None;
	Array<AnimationKeyResource> keys;
};

struct AnimationEventResource
{
	u32 id = 0;
	u32 type = 0;
	f32 time = 0;
};

struct AnimationResource : Resource
{
	AnimationLoopMode loopMode = AnimationLoopMode::Once;
	Array<AnimationTrackResource> tracks;
	Array<AnimationEventResource> events;
};

struct AnimationSetResource : Resource
{
	Array<AnimationResource> animations;
};

}