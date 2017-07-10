#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "gui/types.h"
#include "gui/canvas.h"
#include "base/map.h"
#include "base/array.h"
#include "base/string.h"
#include "base/window_observer.h"

namespace engine
{
using namespace base;
class Widget;
class Canvas;

class ENGINE_API Gui : public WindowObserver
{
public:
	Gui();
	~Gui();

	Canvas& getCanvas() { return canvas; }
	void addRootWidget(Window* window, Widget* widget);
	void removeRootWidget(Widget* widget);
	void update(f32 deltaTime);
	void draw(Window* window);

protected:
	void onWindowEvent(Window* wnd, const WindowEvent& ev) override;
	bool checkWidgetHit(Widget* widget, Widget** widgetHit, const Vec2& pt);
	Widget* findNextFocusableWidget();

	Widget* focusedWidget = nullptr;
	Widget* capturedWidget = nullptr;
	Widget* hoveredWidget = nullptr;
	Map<Window* /*window*/, Widget* /*root widget*/> rootWidgets;
	Canvas canvas;
};

}