#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/class_registry.h"
#include "input/input_device.h"
#include "base/math/vec3.h"
#include "base/math/vec2.h"

namespace engine
{
enum class MouseButton
{
	//! left mouse button
	Left,
	//! right mouse button
	Right,
	//! middle mouse button
	Middle,

	//! max number of mouse buttons
	MaxCount = 16
};

//! A mouse input device
class ENGINE_API Mouse : public InputDevice
{
public:
	friend class InputManager;

	Mouse();

	//! \return true if mouse button is pressed
	bool isButtonDown(u32 button) const
	{
		return buttons[button];
	}
	//! set true if mouse button is pressed
	void setButton(u32 button, bool down)
	{
		buttons[button] = down;
	}

	const Vec2& getMousePosition() const { return position; }
	
	//! set X mouse coordinate
	void setMousePosition(const Vec2& pos)
	{
		position = pos;
	}
	
	//! \return delta values for x,y,z axes
	const Vec3& getDelta() const
	{
		return delta;
	}
	
	//! set delta values for x,y,z axes
	void setDelta(const Vec3& delta)
	{
		this->delta = delta;
	}

protected:
	//! x, y coordinates of the mouse
	Vec2 position;
	//! in x, y is the delta between last move and current\n
	//! in z is the wheel delta
	Vec3 delta;
	//! booleans to indicate which mouse buttons are pressed or not, use BUTTON_LEFT and other values to index in this array
	bool buttons[(int)MouseButton::MaxCount];
};

}