// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "window.h"
#include "base/logger.h"

namespace engine
{
bool InputQueue::peekEvent(size_t index, InputEvent& event)
{
	if (index >= events.size())
	{
		return false;
	}
	
	event = events[index];
	
	return true;
}

bool InputQueue::popEvent(InputEvent& event)
{
	return events.popFirst(event);
}

void InputQueue::addEvent(const InputEvent& event)
{
	events.append(event);
}

void InputQueue::clearEvents()
{
	events.clear();
}

bool InputQueue::hasKeyDownEvent(InputKey key, InputEvent* outEvent)
{
	for (auto& ev : events)
	{
		if (ev.type == InputEvent::EventType::KeyPressed
			&& ev.key.code == key)
		{
			if (outEvent)
				*outEvent = ev;

			return true;
		}
	}
	
	return false;
}

bool InputQueue::hasKeyUpEvent(InputKey key, InputEvent* outEvent)
{
	for (auto& ev : events)
	{
		if (ev.type == InputEvent::EventType::KeyReleased
			&&ev.key.code == key)
		{
			if (outEvent)
				*outEvent = ev;

			return true;
		}
	}

	return false;
}

bool InputQueue::hasMouseButtonDown(MouseButton button, InputEvent* outEvent)
{
	for (auto& ev : events)
	{
		if (ev.type == InputEvent::EventType::MouseButtonPressed
			&& ev.mouseButton.button == button)
		{
			if (outEvent)
				*outEvent = ev;

			return true;
		}
	}

	return false;
}

bool InputQueue::hasMouseButtonUp(MouseButton button, InputEvent* outEvent)
{
	for (auto& ev : events)
	{
		if (ev.type == InputEvent::EventType::MouseButtonReleased
			&& ev.mouseButton.button == button)
		{
			if (outEvent)
				*outEvent = ev;

			return true;
		}
	}

	return false;
}


#ifdef USE_SFML_WINDOW
Window::Window(VideoMode mode, const String& title, WindowStyleFlags style)
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 0;

	sfmlWindow.create({ mode.width, mode.height, mode.bitsPerPixel }, title.c_str(), fromFlags(style), settings);
	sfmlWindow.setActive();
}
Window::Window(WindowHandle handle)
{
	sfmlWindow.create((sf::WindowHandle)handle);
	sfmlWindow.setActive();
}

Window::~Window()
{
	sfmlWindow.close();
}

void Window::close()
{
	sfmlWindow.close();
}

bool Window::isOpen() const
{
	return sfmlWindow.isOpen();
}

bool Window::isFocused() const
{
	return sfmlWindow.hasFocus();
}

bool Window::pollEvent(InputEvent& event)
{
	sf::Event ev;

	bool ret = sfmlWindow.pollEvent(ev);

	event.type = (InputEvent::EventType)ev.type;

	switch (event.type)
	{
		case InputEvent::EventType::WindowResized:
			event.windowResize.width = ev.size.width;
			event.windowResize.height = ev.size.height;
			break;
		case InputEvent::EventType::KeyPressed:
		case InputEvent::EventType::KeyReleased:
			event.key.code = (InputKey)ev.key.code;
			event.key.control = ev.key.control;
			event.key.shift = ev.key.shift;
			event.key.alt = ev.key.alt;
			event.key.system = ev.key.system;
			break;
		case InputEvent::EventType::TextEntered:
			event.text.unicode = ev.text.unicode;
			break;
		case InputEvent::EventType::MouseMoved:
			event.mouseMove.x = ev.mouseMove.x;
			event.mouseMove.y = ev.mouseMove.y;
			break;
		case InputEvent::EventType::MouseButtonPressed:
		case InputEvent::EventType::MouseButtonReleased:
			event.mouseButton.button = (MouseButton)ev.mouseButton.button;
			event.mouseButton.x = ev.mouseButton.x;
			event.mouseButton.y = ev.mouseButton.y;
			break;
		case InputEvent::EventType::MouseWheelMoved:
			event.mouseWheel.delta = ev.mouseWheel.delta;
			event.mouseWheel.x = ev.mouseWheel.x;
			event.mouseWheel.y = ev.mouseWheel.y;
			break;
		case InputEvent::EventType::MouseWheelScrolled:
			event.mouseWheel.delta = ev.mouseWheel.delta;
			event.mouseWheel.x = ev.mouseWheel.x;
			event.mouseWheel.y = ev.mouseWheel.y;
			break;
		case InputEvent::EventType::JoystickButtonPressed:
		case InputEvent::EventType::JoystickButtonReleased:
			event.joystickButton.button = ev.joystickButton.button;
			event.joystickButton.joystickId = ev.joystickButton.joystickId;
			break;
		case InputEvent::EventType::JoystickMoved:
			event.joystickMove.axis = (JoystickAxis)ev.joystickMove.axis;
			event.joystickMove.joystickId = ev.joystickMove.joystickId;
			event.joystickMove.position = ev.joystickMove.position;
			break;
		case InputEvent::EventType::JoystickConnected:
		case InputEvent::EventType::JoystickDisconnected:
			event.joystickConnect.joystickId = ev.joystickConnect.joystickId;
			break;
		case InputEvent::EventType::TouchBegan:
		case InputEvent::EventType::TouchEnded:
		case InputEvent::EventType::TouchMoved:
			event.touch.finger = ev.touch.finger;
			event.touch.position.x = ev.touch.x;
			event.touch.position.y = ev.touch.y;
			break;
		case InputEvent::EventType::SensorChanged:
			event.sensor.type = (SensorType)ev.sensor.type;
			event.sensor.value.x = ev.sensor.x;
			event.sensor.value.y = ev.sensor.y;
			event.sensor.value.y = ev.sensor.z;
			break;
	}

	return ret;
}

Vec2i Window::getPosition() const
{
	auto pos = sfmlWindow.getPosition();

	return { pos.x, pos.y };
}

void Window::setPosition(const Vec2i& position)
{
	sfmlWindow.setPosition({position.x, position.y});
}

Vec2u Window::getSize() const
{
	auto size = sfmlWindow.getSize();

	return { size.x, size.y };
}

void Window::setSize(const Vec2u& size)
{
	sfmlWindow.setSize({ size.x, size.y });
}

void Window::setTitle(const String& title)
{
	sfmlWindow.setTitle(title.c_str());
}

void Window::setIcon(u32 width, u32 height, const u8* pixels)
{
	sfmlWindow.setIcon(width, height, pixels);
}

void Window::setVisible(bool visible)
{
	sfmlWindow.setVisible(visible);
}

void Window::setVerticalSyncEnabled(bool enabled)
{
	sfmlWindow.setVerticalSyncEnabled(enabled);
}

void Window::setMouseCursorVisible(bool visible)
{
	sfmlWindow.setMouseCursorVisible(visible);
}

void Window::setMouseCursorGrabbed(bool grabbed)
{
	sfmlWindow.setMouseCursorGrabbed(grabbed);
}

void Window::setKeyRepeatEnabled(bool enabled)
{
	sfmlWindow.setKeyRepeatEnabled(enabled);
}

void Window::setFramerateLimit(u32 limit)
{
	sfmlWindow.setFramerateLimit(limit);
}

void Window::setJoystickThreshold(f32 threshold)
{
	sfmlWindow.setJoystickThreshold(threshold);
}

bool Window::setActive(bool active) const
{
	return sfmlWindow.setActive(active);
}

void Window::requestFocus()
{
	sfmlWindow.requestFocus();
}

bool Window::hasFocus() const
{
	return sfmlWindow.hasFocus();
}

void Window::present()
{
	sfmlWindow.display();
}

WindowHandle Window::getSystemHandle() const
{
	return (WindowHandle)sfmlWindow.getSystemHandle();
}

void* Window::getImplPtr() const
{
	return (void*)&sfmlWindow;
}

#endif

}