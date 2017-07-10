#pragma once
#include <core/defines.h>
#include <core/types.h>
#include <base/math/color.h>
#include <base/math/rect.h>
#include <base/math/easing.h>
#include <base/math/vec3.h>

namespace engine
{
using namespace base;

struct GuiStateTransitionKey
{
	Color color = Color::white;
	Vec3 position;
	Rect rect;
};

struct ENGINE_API GuiStateTransition
{
	GuiStateTransitionKey keys[2];
	GuiStateTransitionKey currentValue;
	f32 speed = 0.08f;
	f32 time = 0.0f;
	bool reverse = false;
	bool stopped = true;
	bool animateColor = true;
	bool animateRect = true;
	bool animatePositionOnly = true;
	EaseFunction easeFunc = Easing::inLinear;

	void start(bool reversed = false);
	void update(f32 deltaTime);
};

}