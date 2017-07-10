#include "input/input_manager.h"
#include "core/core.h"

namespace engine
{
B_REGISTER_CLASS(InputManager);

InputManager::InputManager()
{}

InputManager::~InputManager()
{}

bool InputManager::initialize()
{
	return true;
}

void InputManager::shutdown()
{}

void InputManager::update()
{}

void InputManager::onWindowEvent(OsWindow* wnd, const WindowEvent& ev)
{
	devices.onWindowEvent(wnd, ev);
}


}