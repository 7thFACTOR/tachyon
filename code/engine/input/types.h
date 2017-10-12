// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once

namespace engine
{
enum class InputDeviceType
{
	None,
	Keyboard,
	Mouse,
	Joystick,
	Sensor,
	Touch,

	Count
};

enum class InputKey
{
	Unknown = -1,
	A = 0,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	Num0,
	Num1,
	Num2,
	Num3,
	Num4,
	Num5,
	Num6,
	Num7,
	Num8,
	Num9,
	Escape,
	LControl,
	LShift,
	LAlt,
	LSystem,
	RControl,
	RShift,
	RAlt,
	RSystem,
	Menu,
	LBracket,
	RBracket,
	SemiColon,
	Comma,
	Period,
	Quote,
	Slash,
	BackSlash,
	Tilde,
	Equal,
	Dash,
	Space,
	Enter,
	BackSpace,
	Tab,
	PageUp,
	PageDown,
	End,
	Home,
	Insert,
	Delete,
	Add,
	Subtract,
	Multiply,
	Divide,
	Left,
	Right,
	Up,
	Down,
	NumPad0,
	NumPad1,
	NumPad2,
	NumPad3,
	NumPad4,
	NumPad5,
	NumPad6,
	NumPad7,
	NumPad8,
	NumPad9,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	Pause,
	LButton,
	MButton,
	RButton,

	Count
};

enum class MouseButton
{
	Left,
	Right,
	Middle,
	XButton1,
	XButton2,

	Count
};

enum class MouseWheel
{
	Vertical,
	Horizontal,

	Count
};

enum class JoystickAxis
{
	X,
	Y,
	Z,
	R,
	U,
	V,
	PovX,
	PovY,

	Count
};

enum class SensorType
{
	Accelerometer,
	Gyroscope,
	Magnetometer,
	Gravity,
	UserAcceleration,
	Orientation,

	Count
};

}