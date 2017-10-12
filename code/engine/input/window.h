// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include <base/defines.h>
#include <base/types.h>
#include <base/array.h>
#include <base/string.h>
#include <base/math/vec2.h>
#include <base/math/vec3.h>
#include <core/defines.h>
#include <core/types.h>
#include <input/types.h>
#include <graphics/types.h>

#ifdef USE_SFML_WINDOW
#ifdef _WINDOWS
#include <windows.h>
#endif
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>
#endif

namespace engine
{
using namespace base;

struct InputEvent
{
	struct WindowResizeEvent
	{
		u32 width = 0;
		u32 height = 0;
	};

	struct KeyEvent
	{
		InputKey code = InputKey::Unknown;
		bool alt = false;
		bool control = false;
		bool shift = false;
		bool system = false;
	};

	struct TextEvent
	{
		u32 unicode = 0;
	};

	struct MouseMoveEvent
	{
		i32 x = 0;
		i32 y = 0;
	};

	struct MouseButtonEvent
	{
		MouseButton button = MouseButton::Left;
		i32 x = 0;
		i32 y = 0;
	};

	struct MouseWheelEvent
	{
		i32 delta = 0;
		i32 x = 0;
		i32 y = 0;
	};

	struct JoystickConnectEvent
	{
		u32 joystickId = 0;
	};

	struct JoystickMoveEvent
	{
		u32 joystickId = 0;
		JoystickAxis axis = JoystickAxis::X;
		f32 position = 0;
	};

	struct JoystickButtonEvent
	{
		u32 joystickId = 0;
		u32 button = 0;
	};

	struct TouchEvent
	{
		u32 finger = 0;
		Vec2i position;
	};

	struct SensorEvent
	{
		SensorType type = SensorType::Accelerometer;
		Vec3f value;
	};

	enum EventType
	{
		WindowClosed,
		WindowResized,
		WindowLostFocus,
		WindowGainedFocus,
		TextEntered,
		KeyPressed,
		KeyReleased,
		MouseWheelMoved,
		MouseWheelScrolled,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseEntered,
		MouseLeft,
		JoystickButtonPressed,
		JoystickButtonReleased,
		JoystickMoved,
		JoystickConnected,
		JoystickDisconnected,
		TouchBegan,
		TouchMoved,
		TouchEnded,
		SensorChanged,

		Count
	};

	EventType type;

	union
	{
		WindowResizeEvent windowResize;
		KeyEvent key;
		TextEvent text;
		MouseMoveEvent mouseMove;
		MouseButtonEvent mouseButton;
		MouseWheelEvent mouseWheel;
		JoystickMoveEvent joystickMove;
		JoystickButtonEvent joystickButton;
		JoystickConnectEvent joystickConnect;
		TouchEvent touch;
		SensorEvent sensor;
	};

	InputEvent() {}
	InputEvent(const InputEvent& other)
	{
		type = other.type;
		windowResize = other.windowResize;
		key = other.key;
		text = other.text;
		mouseMove = other.mouseMove;
		mouseButton = other.mouseButton;
		mouseWheel = other.mouseWheel;
		joystickMove = other.joystickMove;
		joystickButton = other.joystickButton;
		joystickConnect = other.joystickConnect;
		touch = other.touch;
		sensor = other.sensor;
	}

	InputEvent& operator=(const InputEvent& other)
	{
		type = other.type;
		windowResize = other.windowResize;
		key = other.key;
		text = other.text;
		mouseMove = other.mouseMove;
		mouseButton = other.mouseButton;
		mouseWheel = other.mouseWheel;
		joystickMove = other.joystickMove;
		joystickButton = other.joystickButton;
		joystickConnect = other.joystickConnect;
		touch = other.touch;
		sensor = other.sensor;

		return *this;
	}
};

enum class WindowStyleFlags
{
	None = 0, ///< No border / title bar (this flag and all others are mutually exclusive)
	Titlebar = B_BIT(0), ///< Title bar + fixed border
	Resize = B_BIT(1), ///< Title bar + resizable border + maximize button
	Close = B_BIT(2), ///< Title bar + close button
	Fullscreen = B_BIT(3), ///< Fullscreen mode (this flag and all others are mutually exclusive)
	Default = Titlebar | Resize | Close ///< Default window style
};
B_ENUM_AS_FLAGS(WindowStyleFlags);

typedef void* WindowHandle;

class E_API InputQueue
{
public:
	bool peekEvent(size_t index, InputEvent& event);
	bool popEvent(InputEvent& event);
	void addEvent(const InputEvent& event);
	void clearEvents();
	bool hasKeyDownEvent(InputKey key, InputEvent* outEvent = nullptr);
	bool hasKeyUpEvent(InputKey key, InputEvent* outEvent = nullptr);
	bool hasMouseButtonDown(MouseButton button, InputEvent* outEvent = nullptr);
	bool hasMouseButtonUp(MouseButton button, InputEvent* outEvent = nullptr);

protected:
	Array<InputEvent> events;
};


class E_API Window
{
public:
	Window(VideoMode mode, const String& title, WindowStyleFlags style = WindowStyleFlags::Default);
	Window(WindowHandle handle);
	~Window();
	void close();
	bool isOpen() const;
	bool isFocused() const;
	bool pollEvent(InputEvent& event);
	Vec2i getPosition() const;
	void setPosition(const Vec2i& position);
	Vec2u getSize() const;
	void setSize(const Vec2u& size);
	void setTitle(const String& title);
	void setIcon(u32 width, u32 height, const u8* pixels);
	void setVisible(bool visible);
	void setVerticalSyncEnabled(bool enabled);
	void setMouseCursorVisible(bool visible);
	void setMouseCursorGrabbed(bool grabbed);
	void setKeyRepeatEnabled(bool enabled);
	void setFramerateLimit(u32 limit);
	void setJoystickThreshold(f32 threshold);
	bool setActive(bool active = true) const;
	void requestFocus();
	bool hasFocus() const;
	void present();
	WindowHandle getSystemHandle() const;
	void* getImplPtr() const;

#ifdef USE_SFML_WINDOW
	sf::Window sfmlWindow;
#endif
};
}
