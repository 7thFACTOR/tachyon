#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/class_registry.h"
#include "input/input_device.h"
#include "base/math/vec3.h"

namespace engine
{
enum class InputKey
{
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
	Minus,
	Insert,
	Divide,
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
	RButton,
	MButton,
	Pause,
	Greater,
	Lower,
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

struct InputKeyEvent
{
	InputKey key;
};

//! The keyboard device
class ENGINE_API Keyboard : public InputDevice
{
public:
	friend class InputManager;

	//! \return true if key is down/pressed
	bool isKeyDown(InputKey key) const;
	//! \return true if key is up/not pressed
	bool isKeyUp(InputKey key) const;
	//! override key state
	void setKeyState(InputKey key, u8 state);
	const u8* getKeyStatesBuffer() const;
	u32 getKeyStatesBufferSize() const;
	void clearKeyStatesBuffer();

	Keyboard();

protected:
	//! the key states, 1 - pressed, 0 - is up
	u8 keyStates[(u32)InputKey::Count];
};

}