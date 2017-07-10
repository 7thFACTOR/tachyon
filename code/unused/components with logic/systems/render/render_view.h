#pragma once
#include "base/math/rect.h"
#include "base/os_window.h"

namespace engine
{
using namespace base;
struct CameraComponent;

struct IRenderViewEvents
{
	virtual void onBeforeRender(RenderView* pView){}
	virtual void onAfterRender(RenderView* pView){}
};

struct RenderView
{
	OsWindow* pWindow = nullptr;
	Rect bounds;
	bool bActive = true;
	//! lower render order will render sooner
	u32 renderOrder = 0;
	CameraComponent* pCamera = nullptr;
	IRenderViewEvents* pEvents = nullptr;

	RenderView() {}
};

}