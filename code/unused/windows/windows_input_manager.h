#pragma once
#include <windows.h>
#include "input/keyboard.h"
#include "input/mouse.h"
#include "input/joystick.h"
#include "input/input_manager.h"

namespace engine
{
class ENGINE_API WindowsInputManager : public InputManager
{
public:
	B_DECLARE_DERIVED_CLASS(WindowsInputManager, InputManager);
	WindowsInputManager();
	~WindowsInputManager();

	bool initialize() override;
	bool shutdown() override;
	void activate() override;
	void deactivate() override;
	void update() override;
	InputDevice* getDevice(InputDeviceType type) override;
	bool getDevicesOfType(InputDeviceType type, Array<InputDevice*>& devices) override;
	void freeDirectInput();

protected:
	Keyboard keyboardDevice;
	Mouse mouseDevice;
	Joystick joystickDevice;
	POINT mousePos;
};

}