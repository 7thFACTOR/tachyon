#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/math/vec2.h"
#include "base/dictionary.h"

namespace base
{
enum class InputKey
{
	None,
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

	Num1,
	Num2,
	Num3,
	Num4,
	Num5,
	Num6,
	Num7,
	Num8,
	Num9,
	Num0,

	A,
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

	NumLock,
	Scroll,
	NumPad1,
	NumPad2,
	NumPad3,
	NumPad4,
	NumPad5,
	NumPad6,
	NumPad7,
	NumPad8,
	NumPad9,
	NumPad0,
	Multiply,
	Add,
	Subtract,
	Minus,
	Insert,
	Divide,
	Decimal,
	Home,
	End,
	PgUp,
	PgDown,
	ArrowUp,
	ArrowDown,
	ArrowLeft,
	ArrowRight,
	LControl,
	RControl,
	LShift,
	RShift,
	LAlt,
	RAlt,
	Control,
	Shift,
	Alt,
	Tab,
	Space,
	Enter,
	PrintScr,
	Esc,
	CapsLock,
	LButton,
	MButton,
	RButton,
	Pause,
	Backspace,
	LWin,
	RWin,
	Apps,
	Delete,
	Apostrophe,
	Backslash,
	Period,
	Comma,
	Equals,
	LBracket,
	RBracket,
	Semicolon,
	Slash,
	Grave,

	Count
};

enum class InputDeviceType
{
	Unknown,
	Keyboard,
	Mouse,
	Joystick,
	SteeringWheel,
	Pad,
	TouchScreen,
	Sensors,

	Count
};

enum class MouseButton
{
	Left,
	Middle,
	Right,
	Wheel,
	None,
};

union KeyModifiers
{
	KeyModifiers() { flags = 0; }
	KeyModifiers(u32 val) { flags = val; }

	struct
	{
		u32 shift : 1;
		u32 control : 1;
		u32 alt : 1;
		u32 capsLock : 1;
		u32 leftMouseButton : 1;
		u32 middleMouseButton : 1;
		u32 rightMouseButton : 1;
	};

	u32 flags = 0;
};

struct KeyEvent
{
	InputKey code = InputKey::None;
	bool down = false;
	KeyModifiers modifiers;
};

struct MouseEvent
{
	i32 x = 0;
	i32 y = 0;
	i32 wheelDelta = 0;
	MouseButton button = MouseButton::Left;
	i32 clickCount = 1;
	i32 wheelX = 0;
	i32 wheelY = 0;
};

struct TextEvent
{
	static const int maxTextBufferSize = 64;
	char text[maxTextBufferSize] = { 0 };
};

struct TextEditingEvent
{
	static const int maxTextBufferSize = 64;
	char text[maxTextBufferSize] = { 0 };
	i32 start = 0;
	i32 length = 0;
};

struct JoystickEvent
{
	u32 button = 0;
	bool down = false;
	f32 axisDelta = 0.0f;
};

struct GamepadAxisEvent
{
	enum class Axis
	{
		Invalid,
		LeftX,
		LeftY,
		RightX,
		RightY,
		TriggerLeft,
		TriggerRight,
		
		Count
	};

	u32 joystickId = 0;
	Axis axis = Axis::Invalid;
	f32 value = 0; //!< the axis value (range: -1f to 1f)
};

struct GamepadButtonEvent
{
	enum class Button
	{
		Invalid,
		A,
		B,
		X,
		Y,
		Back,
		Guide,
		Start,
		LeftStick,
		RightStick,
		LeftShoulder,
		RightShoulder,
		DPadUp,
		DPadDown,
		DPadLeft,
		DPadRight,

		Count
	};

	bool down = false;
	u32 joystickId = 0;
	Button button = Button::Invalid;
};

struct GamepadDeviceEvent
{
	enum class Type
	{
		None,
		Added,
		Removed,
		Remapped,

		Count
	};

	Type type = Type::None;
	u32 id = 0;
};

struct InputEvent
{
	enum class Type
	{
		None,
		MouseDown,
		MouseUp,
		MouseMove,
		MouseWheel,
		Key,
		Text,
		TextEditing,
		WindowResize,
		WindowGotFocus,
		WindowLostFocus,
		WindowClose
	};

	InputEvent()
	{}

	Type type = Type::None;
	class Window* window = nullptr;
	u32 timestamp = 0;

	union
	{
		MouseEvent mouse;
		KeyEvent key;
		TextEvent text;
		TextEditingEvent textEditing;
		GamepadAxisEvent gamepadAxis;
		GamepadButtonEvent gamepadButton;
		GamepadDeviceEvent gamepadDevice;
		//TODO: rest of devices, steering wheel, touch, Oculus
	};
};

class B_API InputQueue
{
public:
	bool peekEvent(size_t index, InputEvent& event);
	bool popEvent(InputEvent& event);
	void addEvent(const InputEvent& event);
	void clearEvents();
	bool hasKeyDownEvent(InputKey key);
	bool hasKeyUpEvent(InputKey key);
	bool hasMouseButtonDown(MouseButton button);
	bool hasMouseButtonUp(MouseButton button);

protected:
	Array<InputEvent> events;
};

}