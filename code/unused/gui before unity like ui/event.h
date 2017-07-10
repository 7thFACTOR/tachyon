#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "base/window.h"

namespace engine
{
class Widget;
struct WidgetEvent;

enum class MouseButton
{
	None,
	Left,
	Middle,
	Right,
	Wheel
};

enum class WidgetEventType
{
	None,
	GotFocus,
	LostFocus,
	MouseIn,
	MouseOut,
	MouseMove,
	MouseButtonDown,
	MouseButtonUp,
	MouseButtonClick,
	MouseButtonDblClick,
	KeyDown,
	KeyUp,
	TouchDown,
	TouchUp,
	TouchMove,
	_Count
};

struct WidgetEvent
{
	WidgetEventType type = WidgetEventType::None;
	Vec2 point;
	Vec2 windowPoint;
	MouseButton button = MouseButton::None;
	InputKey key = InputKey::None;
	KeyModifiers keyModifiers;
	u32 unicodeChar = 0;
	f32 wheelDelta = 0.0f;
	bool cancelBubble = false;
};

}