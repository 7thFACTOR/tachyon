#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/hardware_input.h"
#include "base/window.h"
#include "core/defines.h"
#include "core/types.h"
#include "base/class_registry.h"

namespace engine
{
using namespace base;

//! The InputProvider handles the input from various devices
class ENGINE_API InputManager : public WindowObserver, public ClassInfo
{
public:
	B_DECLARE_CLASS(InputManager);
	InputManager();
	virtual ~InputManager();

	bool initialize();
	void shutdown();
	void update();
	KeyboardInputQ& getKeyboard() { return devices.keyboard; }
	MouseInputDevice& getMouse() { return devices.mouse; }
	JoystickInputDevice& getJoystick() { return devices.joystick; }
	void clearEventQueues() { devices.clearQueues(); }

protected:
	void onWindowEvent(OsWindow* wnd, const WindowEvent& ev);

	InputDevices devices;
};

}