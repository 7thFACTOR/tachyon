#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/class_registry.h"
#include "input/input_device.h"
#include "base/math/vec3.h"

namespace engine
{
//! A joystick input device
class ENGINE_API Joystick : public InputDevice
{
public:
	static const i32 buttonMaxCount = 32;

	Joystick();

	//! \return true if mouse button is pressed
	bool isButtonDown(u32 button) const
	{
		return buttons[button];
	}

protected:
	//! booleans to indicate which joystick buttons are pressed
	bool buttons[buttonMaxCount];
	//! number of valid joystick buttons in m_bButtons
	u32 buttonCount;
	//! the joystick axis delta values
	f32 axisDelta;
};

}