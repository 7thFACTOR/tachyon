// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include <base/defines.h>
#include <base/types.h>
#include <base/string.h>
#include <base/math/vec2.h>
#include <base/math/vec3.h>
#include <core/defines.h>
#include <core/types.h>
#include <input/types.h>

namespace engine
{
using namespace base;

class Window;

struct E_API JoystickIdentification
{
	String name;
	u32 vendorId;
	u32 productId;
};

class E_API Keyboard
{
public:
	static bool isKeyPressed(InputKey key);
	static void setVirtualKeyboardVisible(bool visible);
};

class E_API Mouse
{
public:
	static bool isButtonPressed(MouseButton button);
	static Vec2i getPosition();
	static Vec2i getPosition(const Window& relativeTo);
	static void setPosition(const Vec2i& position);
	static void setPosition(const Vec2i& position, const Window& relativeTo);
};

class E_API Joystick
{
public:
	enum
	{
		MaxCount = 8,
		MaxButtonCount = 32,
		MaxAxisCount = 8
	};

	static bool isConnected(u32 joystickId);
	static u32 getButtonCount(u32 joystickId);
	static bool hasAxis(JoystickAxis axis, u32 joystickId);
	static bool isButtonPressed(u32 button, u32 joystickId);
	static float getAxisPosition(JoystickAxis axis, u32 joystickId);
	static JoystickIdentification getIdentification(u32 joystickId);
};

class E_API Sensor
{
public:
	static bool isAvailable(SensorType sensor);
	static void setEnabled(SensorType sensor, bool enabled);
	static Vec3f getValue(SensorType sensor);
};

class E_API Touch
{
public:
	static bool isDown(u32 finger);
	static Vec2i getPosition(u32 finger);
	static Vec2i getPosition(u32 finger, const Window& relativeTo);
};

class E_API Input
{
public:
	bool initialize() { return true; }
	void shutdown() {}
};

}