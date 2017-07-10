#pragma once
//-----------------------------------------------------------------------------
// Horus UI Toolkit
//
// (C) 7thFACTOR Software - All rights reserved
//-----------------------------------------------------------------------------

#include <horus/types.h>
#include <horus/defines.h>
#include <horus/rect.h>
#include <string>

namespace horus
{
struct INativeWindow;

enum class KeyCode
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
	RButton,
	MButton,
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

	_Count
};

enum class WindowEventType
{
	Unknown,
	Close,
	Focus,
	LostFocus,
	Capture,
	ReleaseCapture,
	Resize,
	Show,
	Hide,
	Minimize,
	Maximize,
	Restore,
	MouseDown,
	MouseUp,
	MouseMove,
	MouseDblClick,
	KeyDown,
	KeyUp,
	TouchDown,
	TouchUp,
	TouchMove,
	_Count
};

enum class MousePointerType
{
	Default,
	HandOpen,
	HandClosed,
	HandPointing,
	SizeHorizontal,
	SizeVertical,
	SizeDiagonalFromLeft,
	SizeDiagonalFromRight,
	SizeAll,
	TextCaret,
	Wait,
	Denied,
	Cross,
	Custom,
	_Count
};

struct MousePointer
{
	MousePointer(MousePointerType type)
	{
		this->type = type;
		custom = 0;
	}

	MousePointer(Handle customPointerHandle)
	{
		type = MousePointerType::Custom;
		custom = customPointerHandle;
	}

	MousePointer() {}

	MousePointerType type = MousePointerType::Default;
	Handle custom = 0;
};

union KeyModifiers
{
	KeyModifiers() { _flags = 0; }
	KeyModifiers(u32 val) { _flags = val; }

	struct
	{
		u32 shift : 1;
		u32 control : 1;
		u32 alt : 1;
		u32 capsLock : 1;
	};

	u32 _flags = 0;
};

struct NativeWindowListener
{
	virtual void onWindowEvent(
		WindowEventType eventType,
		Handle wnd,
		KeyCode key = KeyCode::None,
		KeyModifiers keyModifiers = 0,
		u32 unicodeChar = 0,
		const Point& relativePoint = Point()) {}
};

struct INativeWindow
{
	virtual Handle handle() const = 0;
	virtual Rect clientRect() const = 0;
	virtual bool isMainWindow() const = 0;
	virtual void maximize() = 0;
	virtual void minimize() = 0;
	virtual void restore() = 0;
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void close() = 0;
	virtual void focus() = 0;
	virtual void capture() = 0;
	virtual void releaseCapture() = 0;
	virtual void setTitle(const std::string& title) = 0;
	virtual void setInternalName(const std::string& name) = 0;
	virtual const std::string& internalName() const = 0;
	virtual void setClientRect(const Rect& rect) = 0;
	virtual void setScreenRect(const Rect& rect) = 0;
	virtual void setListener(NativeWindowListener* listener) = 0;
	virtual void setRenderContext(Handle context) = 0;
	virtual NativeWindowListener* listener() const = 0;
};

HORUS_EXPORT INativeWindow* createNativeWindow(
	const std::string& title,
	const std::string& internalName,
	const Rect& rect,
	bool visible);
HORUS_EXPORT bool handleOsMessages();

}