#include "windows_input_manager.h"
#include "core/core.h"
#include "base/assert.h"
#include "base/logger.h"
#include "render/renderer.h"
#include "base/os_window.h"
#include "base/math/util.h"

namespace engine
{
B_REGISTER_CLASS(WindowsInputManager);

u8 keyboardTable[] =
{
	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',
	VK_NUMLOCK,
	VK_SCROLL,
	VK_NUMPAD1,
	VK_NUMPAD2,
	VK_NUMPAD3,
	VK_NUMPAD4,
	VK_NUMPAD5,
	VK_NUMPAD6,
	VK_NUMPAD7,
	VK_NUMPAD8,
	VK_NUMPAD9,
	VK_NUMPAD0,
	VK_MULTIPLY,
	VK_ADD,
	VK_SUBTRACT,
	VK_INSERT,
	VK_DIVIDE,
	VK_HOME,
	VK_END,
	VK_PRIOR,
	VK_NEXT,
	VK_UP,
	VK_DOWN,
	VK_LEFT,
	VK_RIGHT,
	VK_LCONTROL,
	VK_RCONTROL,
	VK_LSHIFT,
	VK_RSHIFT,
	VK_LMENU,
	VK_RMENU,
	0,
	0,
	0,
	VK_TAB,
	VK_SPACE,
	VK_RETURN,
	0,
	VK_ESCAPE,
	VK_CAPITAL,
	0,
	0,
	0,
	VK_PAUSE,
	0,
	0,
	VK_BACK,
	VK_LWIN,
	VK_RWIN,
	VK_APPS,
	VK_DELETE,
	'\'',
	'\\',
	'.',
	',',
	'=',
	'[',
	']',
	';',
	'/',
	'~'
};

int setKeyboardSpeed(int delay)
{
	/* fastest nDelay = 31, slowest nDelay = 0 */
	return (SystemParametersInfo(SPI_SETKEYBOARDSPEED, delay, nullptr, SPIF_SENDCHANGE) > 0);
}

int setKeyboardDelay(int delay)
{
	/* 0 = shortest (approx 250ms) to 3 longest (approx 1sec) */
	return (SystemParametersInfo(SPI_SETKEYBOARDDELAY, delay, nullptr, SPIF_SENDCHANGE) > 0);
}

WindowsInputManager::WindowsInputManager()
{
	activated = false;
	exclusiveMode = false;
}

WindowsInputManager::~WindowsInputManager()
{}

bool WindowsInputManager::initialize()
{
	B_ASSERT(&getRenderer());

	if (!getRenderer().getOsWindow())
	{
		B_LOG_ERROR("Please set a render window before initialize the InputProvider");
		return false;
	}

	if (!getRenderer().getOsWindow()->getHandle())
	{
		B_LOG_ERROR("Please set a render window (handle) before initialize the InputProvider");
		return false;
	}

	HWND hWnd = (HWND)getRenderer().getOsWindow()->getHandle();

	return true;
}

bool WindowsInputManager::shutdown()
{
	activated = false;
	return true;
}

void WindowsInputManager::update()
{
	HWND hWnd = (HWND)getRenderer().getOsWindow()->getHandle();

	if (!activated)
		return;

	if(GetFocus() != hWnd)
		return;

	for (u32 i = 0; i < 256; ++i)
	{
		keyboardDevice.setKeyState(
			(InputKey)i,
			((GetAsyncKeyState(keyboardTable[i]) & 0x8000) != 0));
	}

	mouseDevice.setButton((int)MouseButton::Left, (GetAsyncKeyState(VK_LBUTTON) & 0x8000));
	mouseDevice.setButton((int)MouseButton::Right, (GetAsyncKeyState(VK_RBUTTON) & 0x8000));
	mouseDevice.setButton((int)MouseButton::Middle, (GetAsyncKeyState(VK_MBUTTON) & 0x8000));

	POINT pt, ptCenter;
	int dx;
	int dy;

	GetCursorPos(&pt);
	
	if (exclusiveMode)
	{
		RECT rc;

		GetWindowRect(hWnd, &rc);

		ptCenter.x = rc.left + (rc.right - rc.left) / 2;
		ptCenter.y = rc.top + (rc.bottom - rc.top) / 2;

		dx = pt.x - ptCenter.x;
		dy = pt.y - ptCenter.y;

		mousePos.x += dx;
		mousePos.y += dy;

		// clamp
		B_ASSERT(getRenderer().getOsWindow());
		Rect wndSize = getRenderer().getOsWindow()->getRect();
		mousePos.x = clampValue((int)mousePos.x, (int)0, (int)wndSize.width);
		mousePos.y = clampValue((int)mousePos.y, (int)0, (int)wndSize.height);
		
		// put cursor in middle of screen
		::SetCursorPos(ptCenter.x, ptCenter.y);
	}
	else
	{
		ScreenToClient(hWnd, &pt);
		dx = pt.x - mousePos.x;
		dy = pt.y - mousePos.y;
		mousePos.x = pt.x;
		mousePos.y = pt.y;
	}

	mouseDevice.setDelta({ (f32)dx, (f32)dy, (f32)0.0f });
	mouseDevice.setMousePosition(Vec2(mousePos.x, mousePos.y));
	//TODO: joystick feeder, using XInput
}

void WindowsInputManager::activate()
{
	if (getEngineMode() == EngineMode::Editing)
	{
		exclusiveMode = false;
	}

	B_ASSERT(&getRenderer());
	B_ASSERT(getRenderer().getOsWindow());
	HWND hWnd = (HWND)getRenderer().getOsWindow()->getHandle();
	POINT ptCenter;
	RECT rc;

	if (exclusiveMode)
	{
		GetWindowRect(hWnd, &rc);
		ptCenter.x = rc.left + (rc.right - rc.left) / 2;
		ptCenter.y = rc.top + (rc.bottom - rc.top) / 2;
		mousePos = ptCenter;
	}
	else
	{
		GetCursorPos(&mousePos);
		ScreenToClient(hWnd, &mousePos);
	}

	mouseDevice.setDelta({ 0, 0, 0 });
	mouseDevice.setMousePosition({ (f32)mousePos.x, (f32)mousePos.y });
	B_LOG_INFO("Activating input on window handle 0x%X...", hWnd);
	SetFocus(hWnd);

	if (exclusiveMode)
	{
		SetCursorPos(ptCenter.x, ptCenter.y);
		ShowCursor(FALSE);
		SetCapture(hWnd);
	}

	//TODO: joystick XInput
	activated = true;
}

void WindowsInputManager::deactivate()
{
	B_LOG_INFO("Deactivating input...");
	
	if (exclusiveMode)
	{
		ShowCursor(TRUE);
		ReleaseCapture();
	}

	activated = false;
}

InputDevice* WindowsInputManager::getDevice(InputDeviceType type)
{
	switch (type)
	{
	case InputDeviceType::Keyboard:
		return &keyboardDevice;
		break;
	case InputDeviceType::Mouse:
		return &mouseDevice;
		break;
	case InputDeviceType::Joystick:
		return &joystickDevice;
		break;
	}

	return nullptr;
}

bool WindowsInputManager::getDevicesOfType(InputDeviceType type, Array<InputDevice*>& devices)
{
	switch (type)
	{
	case InputDeviceType::Keyboard:
		devices.append(&keyboardDevice);
		break;
	case InputDeviceType::Mouse:
		devices.append(&mouseDevice);
		break;
	case InputDeviceType::Joystick:
		devices.append(&joystickDevice);
		break;
	}

	return true;
}

}