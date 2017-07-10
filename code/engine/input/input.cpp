#include "input/input.h"
#include "input/window.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Sensor.hpp>
#include <SFML/Window/Touch.hpp>

namespace engine
{
bool Keyboard::isKeyPressed(InputKey key)
{
	return sf::Keyboard::isKeyPressed((sf::Keyboard::Key)key);
}

void Keyboard::setVirtualKeyboardVisible(bool visible)
{
	sf::Keyboard::setVirtualKeyboardVisible(visible);
}

bool Mouse::isButtonPressed(MouseButton button)
{
	return sf::Mouse::isButtonPressed((sf::Mouse::Button)button);
}

Vec2i Mouse::getPosition()
{
	auto pos = sf::Mouse::getPosition();

	return { pos.x, pos.y };
}

Vec2i Mouse::getPosition(const Window& relativeTo)
{
	sf::Window* wnd = (sf::Window*)relativeTo.getImplPtr();
	auto pos = sf::Mouse::getPosition(*wnd);

	return{ pos.x, pos.y };
}

void Mouse::setPosition(const Vec2i& position)
{
	sf::Mouse::setPosition({ position.x, position.y });
}

void Mouse::setPosition(const Vec2i& position, const Window& relativeTo)
{
	sf::Window* wnd = (sf::Window*)relativeTo.getImplPtr();
	sf::Mouse::setPosition({ position.x, position.y }, *wnd);
}

bool Joystick::isConnected(u32 joystickId)
{
	return sf::Joystick::isConnected(joystickId);
}

u32 Joystick::getButtonCount(u32 joystickId)
{
	return sf::Joystick::getButtonCount(joystickId);
}

bool Joystick::hasAxis(JoystickAxis axis, u32 joystickId)
{
	return sf::Joystick::hasAxis(joystickId, (sf::Joystick::Axis)axis);
}

bool Joystick::isButtonPressed(u32 button, u32 joystickId)
{
	return sf::Joystick::isButtonPressed(joystickId, button);
}

float Joystick::getAxisPosition(JoystickAxis axis, u32 joystickId)
{
	return sf::Joystick::getAxisPosition(joystickId, (sf::Joystick::Axis)axis);
}

JoystickIdentification Joystick::getIdentification(u32 joystickId)
{
	sf::Joystick::Identification jid = sf::Joystick::getIdentification(joystickId);

	return{
		jid.name.toAnsiString().c_str(),
		jid.vendorId,
		jid.productId
	};
}

bool Sensor::isAvailable(SensorType sensor)
{
	return sf::Sensor::isAvailable((sf::Sensor::Type)sensor);
}

void Sensor::setEnabled(SensorType sensor, bool enabled)
{
	sf::Sensor::setEnabled((sf::Sensor::Type)sensor, enabled);
}

Vec3f Sensor::getValue(SensorType sensor)
{
	auto val = sf::Sensor::getValue((sf::Sensor::Type)sensor);

	return { val.x, val.y, val.z };
}

bool Touch::isDown(u32 finger)
{
	return sf::Touch::isDown(finger);
}

Vec2i Touch::getPosition(u32 finger)
{
	auto pos = sf::Touch::getPosition(finger);

	return { pos.x, pos.y };
}

Vec2i Touch::getPosition(u32 finger, const Window& relativeTo)
{
	sf::Window* wnd = (sf::Window*)relativeTo.getImplPtr();
	auto pos = sf::Touch::getPosition(finger, *wnd);

	return { pos.x, pos.y };
}

}