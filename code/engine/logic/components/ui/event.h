#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "base/window.h"

namespace engine
{
enum class UiEventType
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

struct UiEvent
{
	UiEventType type = UiEventType::None;
	Vec2 point;
	MouseButton button = MouseButton::None;
	InputKey key = InputKey::None;
	KeyModifiers keyModifiers;
	u32 unicodeChar = 0;
	f32 wheelDelta = 0.0f;
	bool cancelBubble = false;
};

}