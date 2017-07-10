#include "gui/manager.h>
#include "gui/rect.h>
#include "gui/canvas.h>
#include "gui/widget.h>
#include "gui/native_window.h>
#include "gui/event.h>

#ifdef HORUS_PLATFORM_WIN
#include <windows.h>
#endif

namespace horus
{
static Manager* managerInstance = nullptr;
static u32 lastTime = 0;
static f32 currentDeltaTime = 0.0f;

bool initialize()
{
	managerInstance = new Manager();

#ifdef HORUS_PLATFORM_WIN
	lastTime = GetTickCount();
#endif

	return true;
}

void shutdown()
{
	delete managerInstance;
}

Manager& manager()
{
	return *managerInstance;
}

void updateDeltaTime()
{
	u32 crtTime = 0;

#ifdef HORUS_PLATFORM_WIN
	crtTime = GetTickCount();
#endif
	currentDeltaTime = (f32)(crtTime - lastTime) / 1000.0f;
	lastTime = crtTime;
}

f32 deltaTime()
{
	return currentDeltaTime;
}

void copyToClipboard(const std::string& text)
{
#ifdef HORUS_PLATFORM_WIN
	if (!OpenClipboard(0))
	{
		return;
	}

	EmptyClipboard();

	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, text.length() + 1);

	if (hglbCopy == nullptr)
	{
		CloseClipboard();
		return;
	}

	LPSTR lptstrCopy = (LPSTR)GlobalLock(hglbCopy);

	if (lptstrCopy)
		memcpy(lptstrCopy, text.c_str(), text.length() + 1);

	GlobalUnlock(hglbCopy);
	SetClipboardData(CF_TEXT, hglbCopy);
	CloseClipboard();
#endif
}

std::string pasteFromClipboard()
{
#ifdef HORUS_PLATFORM_WIN
	std::string text = "";

	if (!IsClipboardFormatAvailable(CF_TEXT))
		return "";

	if (!OpenClipboard(0))
	{
		return "";
	}

	HGLOBAL hglb = GetClipboardData(CF_TEXT);

	if (hglb != nullptr)
	{
		LPSTR lptstr = (LPSTR)GlobalLock(hglb);

		if (lptstr != nullptr)
		{
			text = lptstr;
			GlobalUnlock(hglb);
		}
	}

	CloseClipboard();

	return text;
#endif
}

//////////////////////////////////////////////////////////////////////////

Manager::Manager()
{}

Manager::~Manager()
{
	for (auto pair : rootWidgets)
	{
		delete pair.second;
	}
}

void Manager::addRootWidget(Handle window, Widget* widget)
{
	auto iter = rootWidgets.find(window);

	if (iter == rootWidgets.end())
	{
		rootWidgets.insert(std::make_pair(window, new Widget()));
		iter = rootWidgets.find(window);
	}

	iter->second->addChild(widget);
}

void Manager::removeRootWidget(Widget* widget)
{
	for (auto& wnd : rootWidgets)
	{
		wnd.second->removeChild(widget);
	}
}

void Manager::update(f32 deltaTime)
{
	for (auto& pair : rootWidgets)
	{
		pair.second->setRect(canvas.getWindowClientRect());
		pair.second->updateInternal(deltaTime);
	}
}

void Manager::draw(Handle window)
{
	auto iter = rootWidgets.find(window);

	if (iter == rootWidgets.end())
	{
		return;
	}

	iter->second->drawInternal(canvas);
}

void Manager::listenToWindowEvents(INativeWindow* wnd)
{
	if (wnd)
	{
		wnd->setListener((NativeWindowListener*)this);
	}
}

bool Manager::checkWidgetHit(Widget* widget, Widget** widgetHit, const Point& pt)
{
	if (widget->absRect.contains(pt) || !widget->parent)
	{
		*widgetHit = widget;

		if (widget->children.size() != 0)
		{
			// check the widgets from top to bottom, in the order they were added
			for (int i = widget->children.size() - 1; i >= 0; i--)
			{
				if (checkWidgetHit(widget->children[i], widgetHit, pt))
				{
					return true;
				}
			}
		}

		return true;
	}

	return false;
}

Widget* Manager::findNextFocusableWidget()
{
	if (!focusedWidget)
	{
		return nullptr;
	}

	return focusedWidget->handleChangeFocusInChildren(focusedWidget);
}

MouseButton windowButtonToMouseButton(KeyCode key)
{
	switch (key)
	{
	case horus::KeyCode::LButton:
		return MouseButton::Left;
		break;
	case horus::KeyCode::RButton:
		return MouseButton::Right;
		break;
	case horus::KeyCode::MButton:
		return MouseButton::Middle;
		break;
	default:
		break;
	}

	return MouseButton::None;
}

void Manager::onWindowEvent(
	WindowEventType eventType,
	Handle wnd,
	KeyCode key,
	KeyModifiers keyModifiers,
	u32 unicodeChar,
	const Point& relativePoint)
{
	WidgetEvent e;

	e.point = relativePoint;
	e.button = windowButtonToMouseButton(key);
	e.windowPoint = relativePoint;
	e.key = key;
	e.unicodeChar = unicodeChar;
	e.keyModifiers = keyModifiers;

	Widget* hitWidget = nullptr;
	bool hit = false;

	auto iterWnd = rootWidgets.find(wnd);

	if (iterWnd == rootWidgets.end())
	{
		return;
	}

	auto widget = iterWnd->second;

	hitWidget = nullptr;

	// check hit widget, return topmost hit widget
	hit = checkWidgetHit(widget, &hitWidget, relativePoint);

	if (hitWidget)
	{
		WidgetEvent ee;

		// if we have a hovered and its different from the current
		if (hoveredWidget && hoveredWidget != hitWidget)
		{
			ee.type = WidgetEventType::MouseOut;
			ee.cancelBubble = true;
			hoveredWidget->flags.hovered = 0;
			hoveredWidget->handleEventInternal(ee);

			hoveredWidget = hitWidget;
			ee.type = WidgetEventType::MouseIn;
			ee.cancelBubble = true;
			hoveredWidget->flags.hovered = 1;
			hoveredWidget->handleEventInternal(ee);
		}

		if (!hoveredWidget)
		{
			hoveredWidget = hitWidget;
			ee.type = WidgetEventType::MouseIn;
			ee.cancelBubble = true;
			hoveredWidget->flags.hovered = 1;
			hoveredWidget->handleEventInternal(ee);
		}
	}
	else if (hoveredWidget)
	{
		bool isInside = hoveredWidget->absRect.contains(relativePoint);

		WidgetEvent ee;

		if (!isInside)
		{
			ee.type = WidgetEventType::MouseOut;
			ee.cancelBubble = true;
			hoveredWidget->flags.hovered = 0;
			hoveredWidget->handleEventInternal(ee);
			hoveredWidget = nullptr;
		}
	}

	if (capturedWidget)
	{
		hitWidget = capturedWidget;
		hit = true;
	}

	if (!hitWidget)
	{
		hitWidget = focusedWidget;
	}

	if (!focusedWidget && !hitWidget)
	{
		return;
	}

	switch (eventType)
	{
	case horus::WindowEventType::Unknown:
		break;
	case horus::WindowEventType::Close:
		break;
	case horus::WindowEventType::Focus:
		break;
	case horus::WindowEventType::LostFocus:
		break;
	case horus::WindowEventType::Capture:
		break;
	case horus::WindowEventType::ReleaseCapture:
		break;
	case horus::WindowEventType::Resize:
		break;
	case horus::WindowEventType::Show:
		break;
	case horus::WindowEventType::Hide:
		break;
	case horus::WindowEventType::Minimize:
		break;
	case horus::WindowEventType::Maximize:
		break;
	case horus::WindowEventType::Restore:
		break;
	case horus::WindowEventType::MouseDown:
		e.type = WidgetEventType::MouseButtonDown;
		break;
	case horus::WindowEventType::MouseUp:
		e.type = WidgetEventType::MouseButtonUp;
		break;
	case horus::WindowEventType::MouseMove:
		e.type = WidgetEventType::MouseMove;
		break;
	case horus::WindowEventType::MouseDblClick:
		e.type = WidgetEventType::MouseButtonDblClick;
		break;
	case horus::WindowEventType::KeyDown:
		e.type = WidgetEventType::KeyDown;
		break;
	case horus::WindowEventType::KeyUp:
		e.type = WidgetEventType::KeyUp;
		break;
	case horus::WindowEventType::TouchDown:
	case horus::WindowEventType::TouchUp:
		break;
	case horus::WindowEventType::TouchMove:
		break;
	case horus::WindowEventType::Count:
		break;
	default:
		break;
	}

	if (e.type != WidgetEventType::None)
	{
		if (e.type == WidgetEventType::KeyDown
			|| e.type == WidgetEventType::KeyUp)
		{
			if (focusedWidget)
			{
				hitWidget = focusedWidget;
			}
		}

		hitWidget->handleEventInternal(e);

		if (hitWidget->flags.focused)
		{
			if (focusedWidget)
			{
				if (focusedWidget != hitWidget)
				{
					focusedWidget->flags.focused = 0;

					WidgetEvent ee;
					ee.type = WidgetEventType::LostFocus;
					focusedWidget->handleEventInternal(ee);
				}
			}

			focusedWidget = hitWidget;
			focusedWidget->flags.focused = 1;
			WidgetEvent ee;
			ee.type = WidgetEventType::GotFocus;
			focusedWidget->handleEventInternal(ee);
		}

		if (e.key == KeyCode::Tab && e.type == WidgetEventType::KeyUp)
		{
			if (focusedWidget)
			{
				WidgetEvent ee;
				
				ee.type = WidgetEventType::LostFocus;
				focusedWidget->flags.focused = 0;
				focusedWidget->handleEventInternal(ee);
				focusedWidget = findNextFocusableWidget();

				if (focusedWidget)
				{
					ee.type = WidgetEventType::GotFocus;
					focusedWidget->flags.focused = 1;
					focusedWidget->handleEventInternal(ee);
				}
			}
		}

		// if no captured widget is set and it wants to be, then set it
		if (!capturedWidget && hitWidget)
		{
			if (hitWidget->flags.captured)
			{
				capturedWidget = hitWidget;
			}
		}
		else if (capturedWidget == hitWidget)
		{
			if (!hitWidget->flags.captured)
			{
				capturedWidget = nullptr;

				if (!hitWidget->absRect.contains(relativePoint))
				{
					hitWidget->flags.hovered = 0;
				}
			}
		}
	}
}

void Manager::setMousePointer(const MousePointer& mousePointer)
{
	this->mousePointer = mousePointer;
}

}