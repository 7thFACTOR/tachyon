#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/class_registry.h"
#include "core/defines.h"

namespace engine
{
using namespace base;

enum class InputDeviceType
{
	Unknown,
	Keyboard,
	Mouse,
	Joystick,
	Wheel,
	Pad,
	TouchScreen,
	Sensors,
	
	Count
};

//! An input device (keyboard, mouse, joystick, wheel, etc.)
class ENGINE_API InputDevice
{
public:
	InputDevice();
	virtual ~InputDevice();

	//! \return the device's name
	const String& getName() const { return name; }
	//! \return the device's type
	InputDeviceType getType() const { return type; }
	//! set the device's name
	virtual void setName(const String& name);
	//! set the device's type
	virtual void setType(InputDeviceType type);

protected:
	String name;
	InputDeviceType type;
};

}