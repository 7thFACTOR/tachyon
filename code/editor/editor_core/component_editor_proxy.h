#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "core/defines.h"
#include "core/types.h"
#include "input/input.h"
#include "input/types.h"
#include "logic/types.h"

namespace engine
{
struct Component;
class ShapeRenderer;
}

namespace editor
{
using namespace base;
using namespace engine;

struct ComponentEditorProxy
{
	virtual bool isRenderingGizmoOnlyWhenSelected() const { return false; }
	virtual ComponentTypeId getComponentTypeId() const = 0;
	virtual void onComponentGizmoRender(ShapeRenderer* shapeRenderer, Component* component, bool selected) = 0;
	virtual void onComponentGizmoMouseEvent(ShapeRenderer* shapeRenderer, Component* component, InputEvent& mouseEvent) = 0;
};

}