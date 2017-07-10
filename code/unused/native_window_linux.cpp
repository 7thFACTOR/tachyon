#ifdef HORUS_PLATFORM_LINUX 
#include <horus/native_window.h>
#include <horus/event.h>
#include <horus/manager.h>
#include <map>

namespace horus
{
struct LinuxNativeWindow;
static std::map<Window, LinuxNativeWindow*> s_nativeWindows;
bool s_mainWindowSet = false;
std::map<int, KeyCode> mapVKtoKeyCode;

KeyCode translateLinuxKeyToKeyCode(int code)
{
	return mapVKtoKeyCode[code];
}

struct LinuxNativeWindow : INativeWindow
{
	Window hwnd = 0;
	std::string internalName;
	NativeWindowListener* listener = nullptr;
	bool isMainWindow = false;
	HGLRC context = 0;

	~LinuxNativeWindow()
	{
		auto iter = s_nativeWindows.find(hwnd);

		if (iter != s_nativeWindows.end())
		{
			s_nativeWindows.erase(iter);
		}
	}

	Handle handle() const
	{
		return (Handle)m_hwnd;
	}

	Rect clientRect() const
	{
		RECT rc;

		GetClientRect(m_hwnd, &rc);

		return Rect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}


	bool isMainWindow() const
	{
		return m_isMainWindow;
	}

	void maximize()
	{
		ShowWindow(m_hwnd, SW_MAXIMIZE);
	}

	void minimize()
	{
		ShowWindow(m_hwnd, SW_MINIMIZE);
	}

	void restore()
	{
		ShowWindow(m_hwnd, SW_RESTORE);
	}

	void show()
	{
		ShowWindow(m_hwnd, SW_SHOW);
	}

	void hide()
	{
		ShowWindow(m_hwnd, SW_HIDE);
	}

	void close()
	{
		CloseWindow(m_hwnd);
	}

	void focus()
	{
		SetFocus(m_hwnd);
	}

	void capture()
	{
		SetCapture(m_hwnd);
	}

	void releaseCapture()
	{
		ReleaseCapture();
	}

	void setTitle(const std::string& title)
	{
		//TODO: Unicode ?
		SetWindowText(m_hwnd, title.c_str());
	}

	void setInternalName(const std::string& name)
	{
		m_internalName = name;
	}

	const std::string& internalName() const
	{
		return m_internalName;
	}

	void setClientRect(const Rect& rect)
	{
		RECT rc;

		rc.left = rect.x;
		rc.top = rect.y;
		rc.bottom = rect.bottom();
		rc.right = rect.right();

		if (AdjustWindowRect(&rc, WS_POPUPWINDOW, FALSE))
		{
			SetWindowPos(m_hwnd, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOACTIVATE);
		}
	}

	void setScreenRect(const Rect& rect)
	{
		SetWindowPos(m_hwnd, 0, rect.x, rect.y, rect.width, rect.height, SWP_NOACTIVATE);
	}

	void setListener(NativeWindowListener* listener)
	{
		m_listener = listener;
	}

	void setRenderContext(Handle context)
	{
		m_context = (HGLRC)context;
	}

	NativeWindowListener* listener() const
	{
		return m_listener;
	}

};

void registerWndClass()
{
	if (s_wndClassWasRegistered)
	{
		return;
	}

	WNDCLASSEX wcex;

	ZeroMemory(&wcex, sizeof(wcex));

	s_wndClassWasRegistered = true;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wcex.lpfnWndProc = (WNDPROC)horusWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(nullptr);
	wcex.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(nullptr), MAKEINTRESOURCE(iconId));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = kHorusWndClass;
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	mapVKtoKeyCode[0] = KeyCode::None;
	mapVKtoKeyCode[VK_F1] = KeyCode::F1;
	mapVKtoKeyCode[VK_F2] = KeyCode::F2;
	mapVKtoKeyCode[VK_F3] = KeyCode::F3;
	mapVKtoKeyCode[VK_F4] = KeyCode::F4;
	mapVKtoKeyCode[VK_F5] = KeyCode::F5;
	mapVKtoKeyCode[VK_F6] = KeyCode::F6;
	mapVKtoKeyCode[VK_F7] = KeyCode::F7;
	mapVKtoKeyCode[VK_F8] = KeyCode::F8;
	mapVKtoKeyCode[VK_F9] = KeyCode::F9;
	mapVKtoKeyCode[VK_F10] = KeyCode::F10;
	mapVKtoKeyCode[VK_F11] = KeyCode::F11;
	mapVKtoKeyCode[VK_F12] = KeyCode::F12;

	mapVKtoKeyCode[0x31] = KeyCode::Num1;
	mapVKtoKeyCode[0x32] = KeyCode::Num2;
	mapVKtoKeyCode[0x33] = KeyCode::Num3;
	mapVKtoKeyCode[0x34] = KeyCode::Num4;
	mapVKtoKeyCode[0x35] = KeyCode::Num5;
	mapVKtoKeyCode[0x36] = KeyCode::Num6;
	mapVKtoKeyCode[0x37] = KeyCode::Num7;
	mapVKtoKeyCode[0x38] = KeyCode::Num8;
	mapVKtoKeyCode[0x39] = KeyCode::Num9;
	mapVKtoKeyCode[0x30] = KeyCode::Num0;

	mapVKtoKeyCode[0x41] = KeyCode::A;
	mapVKtoKeyCode[0x42] = KeyCode::B;
	mapVKtoKeyCode[0x43] = KeyCode::C;
	mapVKtoKeyCode[0x44] = KeyCode::D;
	mapVKtoKeyCode[0x45] = KeyCode::E;
	mapVKtoKeyCode[0x46] = KeyCode::F;
	mapVKtoKeyCode[0x47] = KeyCode::G;
	mapVKtoKeyCode[0x48] = KeyCode::H;
	mapVKtoKeyCode[0x49] = KeyCode::I;
	mapVKtoKeyCode[0x4A] = KeyCode::J;
	mapVKtoKeyCode[0x4B] = KeyCode::K;
	mapVKtoKeyCode[0x4C] = KeyCode::L;
	mapVKtoKeyCode[0x4D] = KeyCode::M;
	mapVKtoKeyCode[0x4E] = KeyCode::N;
	mapVKtoKeyCode[0x4F] = KeyCode::O;
	mapVKtoKeyCode[0x50] = KeyCode::P;
	mapVKtoKeyCode[0x51] = KeyCode::Q;
	mapVKtoKeyCode[0x52] = KeyCode::R;
	mapVKtoKeyCode[0x53] = KeyCode::S;
	mapVKtoKeyCode[0x54] = KeyCode::T;
	mapVKtoKeyCode[0x55] = KeyCode::U;
	mapVKtoKeyCode[0x56] = KeyCode::V;
	mapVKtoKeyCode[0x57] = KeyCode::W;
	mapVKtoKeyCode[0x58] = KeyCode::X;
	mapVKtoKeyCode[0x59] = KeyCode::Y;
	mapVKtoKeyCode[0x5A] = KeyCode::Z;

	mapVKtoKeyCode[VK_NUMLOCK] = KeyCode::NumLock;
	mapVKtoKeyCode[VK_SCROLL] = KeyCode::Scroll;
	mapVKtoKeyCode[VK_NUMPAD1] = KeyCode::NumPad1;
	mapVKtoKeyCode[VK_NUMPAD2] = KeyCode::NumPad2;
	mapVKtoKeyCode[VK_NUMPAD3] = KeyCode::NumPad3;
	mapVKtoKeyCode[VK_NUMPAD4] = KeyCode::NumPad4;
	mapVKtoKeyCode[VK_NUMPAD5] = KeyCode::NumPad5;
	mapVKtoKeyCode[VK_NUMPAD6] = KeyCode::NumPad6;
	mapVKtoKeyCode[VK_NUMPAD7] = KeyCode::NumPad7;
	mapVKtoKeyCode[VK_NUMPAD8] = KeyCode::NumPad8;
	mapVKtoKeyCode[VK_NUMPAD9] = KeyCode::NumPad9;
	mapVKtoKeyCode[VK_NUMPAD0] = KeyCode::NumPad0;

	mapVKtoKeyCode[VK_MULTIPLY] = KeyCode::Multiply;
	mapVKtoKeyCode[VK_ADD] = KeyCode::Add;
	mapVKtoKeyCode[VK_SUBTRACT] = KeyCode::Subtract;
	mapVKtoKeyCode[VK_INSERT] = KeyCode::Insert;
	mapVKtoKeyCode[VK_DIVIDE] = KeyCode::Divide;
	mapVKtoKeyCode[VK_DECIMAL] = KeyCode::Decimal;

	mapVKtoKeyCode[VK_HOME] = KeyCode::Home;
	mapVKtoKeyCode[VK_END] = KeyCode::End;
	mapVKtoKeyCode[VK_PRIOR] = KeyCode::PgUp;
	mapVKtoKeyCode[VK_NEXT] = KeyCode::PgDown;
	mapVKtoKeyCode[VK_UP] = KeyCode::ArrowUp;
	mapVKtoKeyCode[VK_DOWN] = KeyCode::ArrowDown;
	mapVKtoKeyCode[VK_LEFT] = KeyCode::ArrowLeft;
	mapVKtoKeyCode[VK_RIGHT] = KeyCode::ArrowRight;
	mapVKtoKeyCode[VK_LCONTROL] = KeyCode::LControl;
	mapVKtoKeyCode[VK_RCONTROL] = KeyCode::RControl;
	mapVKtoKeyCode[VK_LSHIFT] = KeyCode::LShift;
	mapVKtoKeyCode[VK_RSHIFT] = KeyCode::RShift;
	mapVKtoKeyCode[VK_LMENU] = KeyCode::LAlt;
	mapVKtoKeyCode[VK_RMENU] = KeyCode::RAlt;
	mapVKtoKeyCode[VK_CONTROL] = KeyCode::Control;
	mapVKtoKeyCode[VK_SHIFT] = KeyCode::Shift;
	mapVKtoKeyCode[VK_MENU] = KeyCode::Alt;
	mapVKtoKeyCode[VK_TAB] = KeyCode::Tab;
	mapVKtoKeyCode[VK_SPACE] = KeyCode::Space;
	mapVKtoKeyCode[VK_RETURN] = KeyCode::Enter;
	mapVKtoKeyCode[VK_PRINT] = KeyCode::PrintScr;
	mapVKtoKeyCode[VK_ESCAPE] = KeyCode::Esc;
	mapVKtoKeyCode[VK_CAPITAL] = KeyCode::CapsLock;
	mapVKtoKeyCode[VK_LBUTTON] = KeyCode::LButton;
	mapVKtoKeyCode[VK_RBUTTON] = KeyCode::RButton;
	mapVKtoKeyCode[VK_MBUTTON] = KeyCode::MButton;
	mapVKtoKeyCode[VK_PAUSE] = KeyCode::Pause;
	mapVKtoKeyCode[VK_BACK] = KeyCode::Backspace;
	mapVKtoKeyCode[VK_LWIN] = KeyCode::LWin;
	mapVKtoKeyCode[VK_RWIN] = KeyCode::RWin;
	mapVKtoKeyCode[VK_APPS] = KeyCode::Apps;
	mapVKtoKeyCode[VK_DELETE] = KeyCode::Delete;
	mapVKtoKeyCode[VK_OEM_7] = KeyCode::Apostrophe;
	mapVKtoKeyCode[VK_OEM_5] = KeyCode::Backslash;
	mapVKtoKeyCode[VK_OEM_PERIOD] = KeyCode::Period;
	mapVKtoKeyCode[VK_OEM_COMMA] = KeyCode::Comma;
	mapVKtoKeyCode[0xbb] = KeyCode::Equals;
	mapVKtoKeyCode[0xbd] = KeyCode::Minus;
	mapVKtoKeyCode[VK_OEM_4] = KeyCode::LBracket;
	mapVKtoKeyCode[VK_OEM_6] = KeyCode::RBracket;
	mapVKtoKeyCode[VK_OEM_1] = KeyCode::Semicolon;
	mapVKtoKeyCode[VK_OEM_2] = KeyCode::Slash;
	mapVKtoKeyCode[VK_OEM_3] = KeyCode::Grave;
}

INativeWindow* createNativeWindow(
	const std::string& title,
	const std::string& internalName,
	const Rect& rect,
	bool visible)
{
	registerWndClass();

	WindowsNativeWindow* wnd = new WindowsNativeWindow;

	if (!s_mainWindowSet)
	{
		s_mainWindowSet = true;
		wnd->m_isMainWindow = true;
	}

	DWORD dwExStyle = ( (wnd->m_isMainWindow ? WS_EX_APPWINDOW : 0) | WS_EX_WINDOWEDGE);
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;

	if (!visible)
	{
		dwStyle &= ~WS_VISIBLE;
	}

	wnd->m_hwnd = CreateWindowEx(
		dwExStyle,
		kHorusWndClass,
		title.c_str(),
		dwStyle,
		rect.left(), rect.y, rect.width, rect.height,
		0, 0,
		(HINSTANCE)GetModuleHandle(NULL), 0);

	if (visible)
	{
		ShowWindow((HWND)wnd->m_hwnd, SW_SHOW);
		UpdateWindow((HWND)wnd->m_hwnd);
	}

	s_nativeWindows[wnd->m_hwnd] = wnd;

	RAWINPUTDEVICE device;
	device.usUsagePage = 0x01;
	device.usUsage = 0x06;
	device.dwFlags = 0;
	device.hwndTarget = wnd->m_hwnd;
	RegisterRawInputDevices(&device, 1, sizeof(device));

	return wnd;
}

KeyModifiers getKeyModifiers(WPARAM wParam)
{
	KeyModifiers mods;

	mods.alt = wParam & MK_ALT ? 1 : 0;
	mods.control = wParam & MK_CONTROL ? 1 : 0;
	mods.shift = wParam & MK_SHIFT ? 1 : 0;
	mods.capsLock = GetAsyncKeyState(VK_CAPITAL) & 0x0001 ? 1 : 0;

	return mods;
}

static LRESULT CALLBACK horusWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	u32 wmId, wmEvent;
	WindowsNativeWindow* wnd = s_nativeWindows[hWnd];

	if (!wnd)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	switch (message)
	{
	case WM_COMMAND:
	{
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	case WM_CAPTURECHANGED:
	{
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(WindowEventType::ReleaseCapture, hWnd);
			wnd->m_listener->onWindowEvent(WindowEventType::Capture, (Handle)lParam);
		}

		break;
	}

	case WM_SETFOCUS:
	{
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(WindowEventType::Focus, hWnd);
		}

		break;
	}

	case WM_KILLFOCUS:
	{
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(WindowEventType::LostFocus, hWnd);
		}
		break;
	}

	case WM_LBUTTONDOWN:
	{
		if (wnd->m_listener)
		{
			SetCapture(hWnd);
			wnd->m_listener->onWindowEvent(
				WindowEventType::MouseDown, hWnd,
				KeyCode::LButton, getKeyModifiers(wParam), 0, Point(LOWORD(lParam), HIWORD(lParam)));
		}

		break;
	}

	case WM_RBUTTONDOWN:
	{
		if (wnd->m_listener)
		{
			SetCapture(hWnd);
			wnd->m_listener->onWindowEvent(
				WindowEventType::MouseDown, hWnd,
				KeyCode::RButton, getKeyModifiers(wParam), 0, Point(LOWORD(lParam), HIWORD(lParam)));
		}

		break;
	}

	case WM_MBUTTONDOWN:
	{
		if (wnd->m_listener)
		{
			SetCapture(hWnd);
			wnd->m_listener->onWindowEvent(
				WindowEventType::MouseDown, hWnd,
				KeyCode::MButton, getKeyModifiers(wParam), 0, Point(LOWORD(lParam), HIWORD(lParam)));
		}

		break;
	}

	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(
				WindowEventType::MouseUp, hWnd,
				KeyCode::LButton, getKeyModifiers(wParam), 0, Point(LOWORD(lParam), HIWORD(lParam)));
		}

		break;
	}

	case WM_RBUTTONUP:
	{
		ReleaseCapture();
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(
				WindowEventType::MouseUp, hWnd,
				KeyCode::RButton, getKeyModifiers(wParam), 0, Point(LOWORD(lParam), HIWORD(lParam)));
		}

		break;
	}

	case WM_MBUTTONUP:
	{
		ReleaseCapture();
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(
				WindowEventType::MouseUp, hWnd,
				KeyCode::MButton, getKeyModifiers(wParam), 0, Point(LOWORD(lParam), HIWORD(lParam)));
		}

		break;
	}

	case WM_MOUSEMOVE:
	{
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(
				WindowEventType::MouseMove, hWnd,
				KeyCode::None, getKeyModifiers(wParam), 0, Point(LOWORD(lParam), HIWORD(lParam)));
		}

		break;
	}

	case WM_LBUTTONDBLCLK:
	{
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(
				WindowEventType::MouseDblClick, hWnd,
				KeyCode::LButton, getKeyModifiers(wParam), 0, Point(LOWORD(lParam), HIWORD(lParam)));
		}

		break;
	}

	case WM_RBUTTONDBLCLK:
	{
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(
				WindowEventType::MouseDblClick, hWnd,
				KeyCode::RButton, getKeyModifiers(wParam), 0, Point(LOWORD(lParam), HIWORD(lParam)));
		}

		break;
	}

	case WM_MBUTTONDBLCLK:
	{
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(
				WindowEventType::MouseDblClick, hWnd,
				KeyCode::MButton, getKeyModifiers(wParam), 0, Point(LOWORD(lParam), HIWORD(lParam)));
		}

		break;
	}
	
	case WM_INPUT:
	{
		if (wnd->m_listener)
		{
			char buffer[sizeof(RAWINPUT)] = {};
			UINT size = sizeof(RAWINPUT);
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));

			// extract keyboard raw input data
			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer);
			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				const RAWKEYBOARD& rawKB = raw->data.keyboard;
				UINT virtualKey = rawKB.VKey;
				UINT scanCode = rawKB.MakeCode;
				UINT flags = rawKB.Flags;

				if (virtualKey == 255)
				{
					// discard "fake keys" which are part of an escaped sequence
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
				else if (virtualKey == VK_SHIFT)
				{
					// correct left-hand / right-hand SHIFT
					virtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
				}
				else if (virtualKey == VK_NUMLOCK)
				{
					// correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
					scanCode = (MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC) | 0x100);
				}
				
				// e0 and e1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
				// see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
				const bool isE0 = ((flags & RI_KEY_E0) != 0);
				const bool isE1 = ((flags & RI_KEY_E1) != 0);

				if (isE1)
				{
					// for escaped sequences, turn the virtual key into the correct scan code using MapVirtualKey.
					// however, MapVirtualKey is unable to map VK_PAUSE (this is a known bug), hence we map that by hand.
					if (virtualKey == VK_PAUSE)
						scanCode = 0x45;
					else
						scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
				}

				// a key can either produce a "make" or "break" scancode. this is used to differentiate between down-presses and releases
				// see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
				const bool wasUp = ((flags & RI_KEY_BREAK) != 0);
				static KeyModifiers mods;

				if (virtualKey == VK_LCONTROL || virtualKey == VK_RCONTROL || virtualKey == VK_CONTROL)
					mods.control = wasUp ? 0 : 1;
				
				if (virtualKey == VK_LSHIFT || virtualKey == VK_RSHIFT || virtualKey == VK_SHIFT)
					mods.shift = wasUp ? 0 : 1;
				
				if (virtualKey == VK_LMENU || virtualKey == VK_RMENU || virtualKey == VK_MENU)
					mods.alt = wasUp ? 0 : 1;
				
				bool caps = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
				mods.capsLock = (caps) ? 1 : 0;

				if (wasUp)
				{
					wnd->m_listener->onWindowEvent(
						WindowEventType::KeyUp, hWnd,
						translateWinKeyToKeyCode(virtualKey),
						mods, 0);
				}
				else
				{
					wnd->m_listener->onWindowEvent(
						WindowEventType::KeyDown, hWnd,
						translateWinKeyToKeyCode(virtualKey),
						mods, 0);
				}

			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	case WM_KEYUP:
	{
		if (wnd->m_listener)
		{
			//wnd->m_listener->onWindowEvent(WindowEventType::KeyUp, hWnd, translateWinKeyToKeyCode(wParam), getKeyModifiers(wParam), unicodeCharFromVk(wParam));
		}
		break;
	}

	case WM_MOVE:
	{
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(WindowEventType::Resize, hWnd);
		}

		break;
	}

	case WM_SHOWWINDOW:
	{
		if (wParam)
		{
			if (wnd->m_listener)
			{
				wnd->m_listener->onWindowEvent(WindowEventType::Show, hWnd);
			}
		}
		else
		{
			if (wnd->m_listener)
			{
				wnd->m_listener->onWindowEvent(WindowEventType::Hide, hWnd);
			}
		}

		break;
	}

	case WM_SIZE:
	{
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(WindowEventType::Resize, hWnd);
		}

		break;
	}

	case WM_SETCURSOR:
	{
		LPSTR id = IDC_ARROW;

		switch (manager().getMousePointer().type)
		{
		case horus::MousePointerType::Default:
			break;
		case horus::MousePointerType::HandOpen:
			break;
		case horus::MousePointerType::HandClosed:
			break;
		case horus::MousePointerType::HandPointing:
			id = IDC_HAND;
			break;
		case horus::MousePointerType::SizeHorizontal:
			id = IDC_SIZEWE;
			break;
		case horus::MousePointerType::SizeVertical:
			id = IDC_SIZENS;
			break;
		case horus::MousePointerType::SizeDiagonalFromLeft:
			id = IDC_SIZENWSE;
			break;
		case horus::MousePointerType::SizeDiagonalFromRight:
			id = IDC_SIZENESW;
			break;
		case horus::MousePointerType::SizeAll:
			id = IDC_SIZEALL;
			break;
		case horus::MousePointerType::TextCaret:
			id = IDC_IBEAM;
			break;
		case horus::MousePointerType::Wait:
			id = IDC_WAIT;
			break;
		case horus::MousePointerType::Denied:
			id = IDC_NO;
			break;
		case horus::MousePointerType::Cross:
			id = IDC_CROSS;
			break;
		case horus::MousePointerType::Custom:
			id = MAKEINTRESOURCE(manager().getMousePointer().custom);
			break;
		default:
			break;
		}

		if (id != IDC_ARROW)
		{
			HCURSOR cursor = LoadCursor(NULL, id);
			SetCursor(cursor);
		}
		else
			DefWindowProc(hWnd, message, wParam, lParam);

		break;
	}

	case WM_CLOSE:
	{
		if (wnd->m_listener)
		{
			wnd->m_listener->onWindowEvent(WindowEventType::Close, hWnd);
		}

		if (wnd->isMainWindow())
		{
			PostQuitMessage(0);
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
		
		break;
	}

	case WM_DESTROY:
	{
		break;
	}

	default:
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	}

	return 0;
}


bool handleOsMessages()
{
	MSG msg;
	bool stopped = false;

	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE) && !stopped)
	{
		if (msg.message == WM_QUIT || msg.message == WM_CLOSE)
		{
			stopped = true;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return !stopped;
}

INativeWindow* createNativeWindow(
	const std::string& title,
	const std::string& internalName,
	const Rect& rect,
	bool visible)
{
	return nullptr;
}

bool handleOsMessages()
{
	bool stopped = false;
	return !stopped;
}

}
#endif