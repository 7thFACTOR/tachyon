#include "editor.h"
#include "base/variant.h"
#include "base/parameter_list.h"
#include "base/logger.h"
#include "input/window.h"
#include "graphics/graphics.h"
#include "core/globals.h"
#include "event.h"
#include "history.h"
#include "base/math/conversion.h"
#include "graphics/shape_renderer.h"
#include "logic/logic.h"
#include "logic/components/render/camera.h"

namespace editor
{
using namespace engine;
using namespace base;
//struct Event
//{
//	int type;
//};
//
//struct Widget
//{
//	Widget(const String& name)
//	{}
//
//	Widget& setVisible(bool v) { visible = v; return *this; }
//	Widget& setActive(bool v) { active = v; return *this; }
//	Widget& setFocused(bool v) { focused = v; return *this; }
//	Widget& setDefaultText(const String& dt) { return *this; }
//	
//	inline bool is(const String& n) { return n == name; }
//
//	String tag;
//	String name;
//	int visible : 1;
//	int active : 1;
//	int focused : 1;
//};
//
//struct HorusUi;
//
//typedef void(*UiEventDelegate)(HorusUi& ui, Widget& w, Event& ev);
//
//enum class UiEventFilter
//{
//	All,
//	Click,
//	MouseMove,
//	MouseDown,
//	MouseUp,
//	KeyDown,
//	KeyUp,
//	Key,
//	TouchDown,
//	TouchUp
//};
//
//struct HorusUi
//{
//	Widget& beginPanel(const String& name)
//	{
//		Widget* pnl = new Widget("");
//
//		return *pnl;
//	}
//
//	void endPanel()
//	{
//
//	}
//
//	Widget& beginGroup(const String& name)
//	{
//		return *new Widget("");
//	}
//		
//	void endGroup()
//	{
//	}
//
//	Widget& addButton(const String& n, const String& v = "")
//	{
//		return *new Widget(n);
//	}
//	Widget& addEditBox(const String& n, const String& v = "")
//	{
//		return *new Widget(n);
//	}
//
//	void delegateEvents(UiEventDelegate fn, UiEventFilter filter)
//	{
//
//	}
//
//	void show(const String& n){}
//	void hide(const String& n){}
//
//};
//
//void onWidgetClickEvents(HorusUi& ui, Widget& widget, Event& ev)
//{
//	if (widget.is("btn1"))
//	{
//		ui.hide("edit1");
//		//executeEndOfTheWorld();
//	}
//	else if (widget.is("btn2"))
//	{
//		ui.show("animation_editor");
//	}
//}
//
//void onAllWidgetEvents(HorusUi& ui, Widget& widget, Event& ev)
//{
//	if (widget.is("combo1"))
//	{
//
//		if (ev.type == ComboBoxEventType::SelectionChanged)
//		{
//			updateStuff();
//		}
//	}
//}
//
//void createInspectorPanel(HorusUi& ui)
//{
//	ui.beginPanel("Inspector");
//		ui.addButton("btn1")
//			.setVisible(1)
//			.setFocused(0)
//			.setActive(1);
//		ui.addButton("btn2", "PUSHME");
//		ui.addEditBox("ed0")
//			.setDefaultText("Type here the search string");
//		ui.beginGroup("grp1");
//			ui.addEditBox("ed1", "");
//			ui.addEditBox("ed2", "");
//			ui.addEditBox("ed3", "");
//		ui.endGroup();
//		ui.beginVerticalLayout()
//			ui.beginHorizontalLayout()
//
//			ui.beginGridLayout();
//		ui.beginToolbar
//	ui.endPanel();
//
//	ui.delegateEvents(onWidgetClickEvents, UiEventFilter::Click);
//	ui.delegateEvents(onAllWidgetEvents, UiEventFilter::All);
//}
	
Editor::Editor()
{
	enableHistory = true;
}

Editor::~Editor()
{
}

void Editor::initialize()
{
	shapeRenderer = new ShapeRenderer();
	getLogic().inputMap.load("../config/app.input");
	mainWorld = getLogic().createWorld();
	getLogic().setActiveWorld(mainWorld);
	RenderComponentUpdater* rcu = (RenderComponentUpdater*)getLogic().getComponentUpdater(StdComponentUpdaterId_Render);
	
	rcu->addObserver(this);
}

void Editor::shutdown()
{
	delete shapeRenderer;
}

void Editor::update()
{
}

void Editor::onAfterWorldRender()
{
	renderGizmos();
}

void Editor::renderGizmos()
{
	shapeRenderer->begin();
	getGraphics().setWorldMatrix(Matrix());
	//renderer().setProjectionMatrix(camera->projectionMatrix());
	//renderer().setViewMatrix(camera->viewMatrix());
	//shapeRenderer->drawGrid(5000, 10, 60, Color::black, Color::darkGray, Color::black);
	shapeRenderer->setColor({ 1, 1, 1, 1 });
	getGraphics().rasterizerState.cullMode = CullMode::CCW;
	shapeRenderer->drawSolidBox(55, 55, 55);
	shapeRenderer->drawSolidEllipsoid(50, 50, 50, 25, 25);
	shapeRenderer->drawSimpleAxis(1000);
	Matrix m;
	//shapeRenderer->set2dProjection(0, 0, 1024, 768);
	//shapeRenderer->draw2dTexturedQuad(1328338884, 0, 0, 77, 75);
	shapeRenderer->end();
}

void Editor::pushHistoryAction(EditorHistoryAction* pAction)
{
	if (!enableHistory)
	{
		return;
	}

	history.appendUnique(pAction);
}

void Editor::setEnableHistory(bool value)
{
	enableHistory = value;
}

bool Editor::isHistoryEnabled() const
{
	return enableHistory;
}

bool Editor::undo(u32 steps)
{
	return true;
}

bool Editor::redo(u32 steps)
{
	return true;
}

size_t Editor::historyActionCount() const
{
	return history.size();
}

EditorHistoryAction* Editor::historyActionAt(size_t index) const
{
	return history[index];
}

void Editor::clearHistory()
{
	for(auto item : history)
	{
		delete item;
	}

	history.clear();
}

bool Editor::registerEvent(EditorEvent* pEvent)
{
	events.appendUnique(pEvent);
	return true;
}

bool Editor::unregisterEvent(EditorEvent* pEvent)
{
	auto iter = events.find(pEvent);
	
	if (iter == events.end())
	{
		return false;
	}
	
	events.erase(iter);
	
	return true;
}

bool Editor::isEventRegistered(const String& eventName)
{
	auto iter = events.begin();
	
	while (iter != events.end())
	{
		if ((*iter)->getName() == eventName)
		{
			return true;
		}

		++iter;
	}
	
	return false;
}

void Editor::registerComponentEditorProxy(struct ComponentEditorProxy* proxy)
{
	componentEditorProxies[proxy->getComponentTypeId()] = proxy;
}

void Editor::unregisterComponentEditorProxy(struct ComponentEditorProxy* proxy)
{
	componentEditorProxies.erase(proxy->getComponentTypeId());
}

bool Editor::triggerEvent(EditorEvent* pEvent, void* pUserData)
{
	auto iter = events.find(pEvent);
	
	if (iter == events.end())
	{
		return false;
	}
	
	// call the sinks before
	callEventSinks(pEvent, TriggerContext::Before, pUserData);
	
	// call the event
	(*iter)->onTrigger(pUserData);

	// call the sinks after
	callEventSinks(pEvent, TriggerContext::After, pUserData);

	return true;
}

bool Editor::triggerEvent(const String& eventName, void* pUserData)
{
	EditorEvent* pEvent = nullptr;

	for (auto event : events)
	{
		if (event->getName() == eventName)
		{
			pEvent = event;
			break;
		}
	}

	if (!pEvent)
	{
		return false;
	}
	
	return triggerEvent(pEvent, pUserData);
}

void Editor::callEventSinks(EditorEvent* pEvent, TriggerContext context, void* pUserData)
{
	auto iter = eventSinks.begin();
	
	while (iter != eventSinks.end())
	{
		(*iter)->onEvent(pEvent, context, pUserData);
		++iter;
	}
}

bool Editor::registerEventSink(EditorEventSink* pEventSink)
{
	eventSinks.appendUnique(pEventSink);
	return true;
}

bool Editor::unregisterEventSink(EditorEventSink* pEventSink)
{
	auto iter = eventSinks.find(pEventSink);
	
	if (iter == eventSinks.end())
	{
		return false;
	}
	
	eventSinks.erase(iter);
	return true;
}

//void Editor::onWindowEvent(Window* wnd, const WindowEvent& ev)
//{
//	switch (ev.type)
//	{
//	case WindowEventType::MouseDoubleClick:
//	case WindowEventType::MouseDown:
//	case WindowEventType::MouseUp:
//		{
//			getGraphics().getWindow()->focus();
//			break;
//		}
//	case WindowEventType::WindowResized:
//		{
//			getGraphics().resetViewport();
//			break;
//		}
//	}
//}

}