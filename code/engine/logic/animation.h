#pragma once
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/string.h"
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/types.h"
#include "core/defines.h"
#include "base/assert.h"
#include "core/resources/animation_resource.h"

namespace engine
{
using namespace base;

struct AnimationKeyBase
{
	virtual ~AnimationKeyBase()
	{}

	virtual void interpolate(
		AnimationInterpolation ipolType,
		const AnimationKeyBase* otherKey,
		f32 time,
		void* outValue) = 0;

	f32 time = 0;
	f32 easeIn = 0;
	f32	easeOut = 0;
	f32 tension = 0.5f;
	f32 continuity = 0.5f;
	f32 bias = 0.5f;
};

template<typename Type>
class E_API AnimationKeyTpl : public AnimationKeyBase
{
public:
	AnimationKeyTpl()
	{}

	AnimationKeyTpl(f32 newTime, const Type& newValue)
	{
		value = newValue;
		time = newTime;
	}
	
	~AnimationKeyTpl()
	{}

	void interpolate(
		AnimationInterpolation ipolType,
		const AnimationKeyBase* otherKey,
		f32 atTime,
		void* outValue)
	{
		B_ASSERT(outValue);
		Type outKeyVal;

		switch (ipolType)
		{
		case AnimationInterpolation::Linear:
			{
				const AnimationKeyTpl* k2 = (AnimationKeyTpl*)otherKey;

				outKeyVal = value + (k2->value() - value) * atTime;
				break;
			}

		case AnimationInterpolation::Bezier:
		case AnimationInterpolation::TCB:
			{
				f32 t2, t3, t4, t5, ta, tb, tc, td;

				t2 = atTime * atTime;
				t3 = atTime * t2;
				t4 = 3.0f * t2;
				t5 = 2.0f * t3;
				ta = t5 - t4 + 1.0f;
				tb = t3 - 2.0f * t2 + atTime;
				tc = -t5 + t4;
				td = t3 - t2;

				const AnimationKeyTpl* k2 = (AnimationKeyTpl*)otherKey;

				outKeyVal = value * ta + rightTangent * tb + k2->value * tc + k2->leftTangent * td;
				break;
			}
		}

		if (outValue)
		{
			*(Type*)outValue = outKeyVal;
		}
	}

	Type value;
	Type leftTangent;
	Type rightTangent;
};

struct E_API AnimationTrackBase
{
	AnimationTrackBase()
	{
		speed = 1.0f;
		direction = 1.0f;
		loopMode = AnimationLoopMode::Once;
		interpolation = AnimationInterpolation::TCB;
	}

	virtual ~AnimationTrackBase()
	{}

	virtual void interpolate(f32 time, void* outValue) = 0;
	virtual void computeTangents() = 0;

	String name;
	AnimationInterpolation interpolation;
	AnimationLoopMode loopMode;
	f32 speed;
	//! the animation direction 1 = normal (forward), -1 = reversed, backwards
	f32 direction;
	Array<AnimationKeyBase*> keys;
};

template<typename Type>
class E_API AnimationTrack : public AnimationTrackBase
{
public:
	AnimationTrack()
	{}

	~AnimationTrack()
	{}

	void addKey(f32 time, const Type& value)
	{
		keys.append(new AnimationKeyTpl<Type>(time, value));
	}

	void interpolate(f32 time, void* outValue)
	{
		B_ASSERT(outValue);

		if (!outValue)
		{
			return;
		}

		time *= speed;

		if (keys.size() <= 1)
		{
			return;
		}

		if (time > keys[keys.size() - 1]->time)
		{
			return;
		}

		AnimationKeyBase *key1, *key2;
		key1 = key2 = nullptr;

		// default first key
		key1 = keys[0];

		B_ASSERT(key1);

		// search first key
		for (size_t i = 0, iCount = keys.size(); i < iCount; ++i)
		{
			if (keys[i]->time > key1->time
				&& keys[i]->time <= time)
			{
				key1 = keys[i];
			}
		}

		// default second key
		key2 = keys[keys.size() - 1];

		B_ASSERT(key2);

		// search for the second key
		for (size_t i = 0, iCount = keys.size(); i < iCount; ++i)
		{
			if (keys[i]->time < key2->time
				&& keys[i]->time >= time
				&& key1->time < keys[i]->time)
			{
				key2 = keys[i];
			}
		}

		// if one of the keys not found, return
		if (!key1 || !key2)
		{
			return;
		}

		// compute the local time between the two keys
		f32 localTime = (f32)(time - key1->time) / (key2->time - key1->time);

		// interpolate the keys putting the result into our variable
		key1->interpolate(interpolation, key2, localTime, outValue);
	}

	void computeTangents()
	{
		AnimationKeyTpl<Type> *h1, *h2;

		if (keys.size() <= 1)
		{
			return;
		}

		if (keys.size() == 2)
		{
			h1 = (AnimationKeyTpl<Type>*)keys[0];
			h2 = (AnimationKeyTpl<Type>*)keys[1];
			h1->rightTangent = (h2->value - h1->value) * (1.0f - h1->tension);
			h2->leftTangent = (h2->value - h1->value) * (1.0f - h2->tension);

			return;
		}

		AnimationKeyTpl<Type> *a, *b, *c;
		f32 d1, d2, d;
		f32 adjust1, adjust2;

		for (size_t i = 1, iCount = keys.size() - 1; i < iCount; ++i)
		{
			a = (AnimationKeyTpl<Type>*)keys[i - 1];
			b = (AnimationKeyTpl<Type>*)keys[i];
			c = (AnimationKeyTpl<Type>*)keys[i + 1];
			d1 = b->time - a->time;
			d2 = c->time - b->time;
			d = d1 + d2;

			if (fabs(d - 0.0f) < 0.0001f)
			{
				d = 1.0f;
			}

			adjust1 = (f32) d1 / d;
			adjust2 = (f32) d2 / d;
			adjust1 = 0.5f + (1.0f - fabsf(b->continuity)) * (adjust1 - 0.5f);
			adjust2 = 0.5f + (1.0f - fabsf(b->continuity)) * (adjust2 - 0.5f);

			b->leftTangent =
				((b->value - a->value) * (1.0f + b->bias) * (1.0f - b->continuity)
				+ (c->value - b->value) * (1.0f - b->bias) * (1.0f + b->continuity))
				* (1.0f - b->tension()) * adjust1;
			b->rightTangent =
				((b->value - a->value) * (1.0f + b->bias) * (1.0f + b->continuity)
				+ (c->value - b->value) * (1.0f - b->bias) * (1.0f - b->continuity))
				* (1.0f - b->tension) * adjust2;
		}

		AnimationKeyTpl<Type>* key;
		u32 n = keys.size() - 1;

		key = (AnimationKeyTpl<Type>*)keys[1];
		key->leftTangent = key->value;
		((AnimationKeyTpl<Type>*)keys[0])->rightTangent =
			((((AnimationKeyTpl<Type>*)keys[1])->value - ((AnimationKeyTpl<Type>*)keys[0])->value) * 1.5f - key->value * 0.5f)
			* (1.0f - ((AnimationKeyTpl<Type>*)keys[0])->tension);

		key = (AnimationKeyTpl<Type>*)keys[n - 1];
		key->rightTangent = key->value;
		((AnimationKeyTpl<Type>*)keys[n])->leftTangent =
			((((AnimationKeyTpl<Type>*)keys[n - 1])->value - ((AnimationKeyTpl<Type>*)keys[n])->value) * 1.5f - key->value * 0.5f)
			* (1.0f - ((AnimationKeyTpl<Type>*)keys[0])->tension);
	}
};

class AnimationEvent
{
public:
	virtual ~AnimationEvent() {}

	virtual void onTrigger() = 0;

	f32 time = 0.0f;
	bool active = false;
	bool triggered = false;
};

class E_API Animation
{
public:
	Animation();
	virtual ~Animation();

	void update(f32 deltaTime);
	void addTrack(AnimationTrackBase* track);
	void play();
	AnimationTrackBase* findTrack(const String& name) const;

	f32 duration = 1;
	f32 time = 0;
	f32 timeScale = 1;
	f32 direction = 1;
	AnimationLoopMode loopMode = AnimationLoopMode::Once;
	Array<AnimationTrackBase*> tracks;
	Array<AnimationEvent*> events;
	bool autoDuration = true;
};

typedef AnimationKeyTpl<f32> AnimationKeyF32;
typedef AnimationKeyTpl<f64> AnimationKeyF64;
typedef AnimationKeyTpl<i32> AnimationKeyI32;
typedef AnimationKeyTpl<u32> AnimationKeyU32;
typedef AnimationKeyTpl<Vec3> AnimationKeyVec3;

typedef AnimationTrack<f32> AnimationTrackF32;
typedef AnimationTrack<f64> AnimationTrackF64;
typedef AnimationTrack<i32> AnimationTrackI32;
typedef AnimationTrack<u32> AnimationTrackU32;
typedef AnimationTrack<Vec3> AnimationTrackVec3;

}