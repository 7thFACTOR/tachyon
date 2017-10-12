// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/math/rect.h"
#include "input/window.h"

namespace base
{
class Window;
using namespace base;
struct CameraComponent;

class RenderViewEvents
{
public:
	virtual void onBeforeRender(RenderView* view){}
	virtual void onAfterRender(RenderView* view){}
};

struct RenderView
{
	Window* window = nullptr;
	Rect bounds;
	bool active = true;
	//! lower render order will render first
	u32 renderOrder = 0;
	CameraComponent* camera = nullptr;
	RenderViewEvents* events = nullptr;

	RenderView() {}
};

}