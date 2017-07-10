#include "editor/editor.h"
#include "base/variant.h"
#include "base/parameter_list.h"
#include "base/logger.h"
#include "base/os_window.h"
#include "render/render_manager.h"
#include "core/core.h"
#include "editor/event.h"
#include "editor/history.h"
#include "editor/command.h"
#include "editor/editor_server.h"
#include "base/math/conversion.h"
#include "input/input_manager.h"
#include "input/mouse.h"
#include "render/aux_renderer.h"
#include "game/game_manager.h"
#include "render/scene/scene.h"
#include "render/scene/camera_node.h"

namespace engine
{
B_REGISTER_CLASS(Editor); 

Editor::Editor()
{
	m_bEnableHistory = true;
}

Editor::~Editor()
{
}

void Editor::initialize()
{
	Array<IClassDescriptor*> cmdClasses;

	findDerivedClasses("EditorCommand", cmdClasses);

	for (size_t i = 0; i < cmdClasses.size(); ++i)
	{
		B_LOG_INFO("Registering editor command: %s", cmdClasses[i]->className().c_str());
		registerCommand((EditorCommand*)cmdClasses[i]->newInstance());
	}

	render().osWindow()->addObserver(this);

	m_pServer = new EditorServer;
	m_pServer->startServer();
}

void Editor::shutdown()
{
	render().osWindow()->removeObserver(this);
	m_pServer->stopServer();
	delete m_pServer;
}

void Editor::update()
{
	m_pServer->update();
}

void Editor::renderGizmos()
{
	auto scene = render().scene();
	auto camera = scene->camera();

	if (!camera)
	{
		return;
	}

	auxRenderer()->begin();
	render().setWorldMatrix(Matrix());
	render().setProjectionMatrix(camera->projectionMatrix());
	render().setViewMatrix(camera->viewMatrix());
	auxRenderer()->drawGrid(5000, 10, 60, Color::kBlack, Color::kDarkGray, Color::kBlack);
	auxRenderer()->setColor(1, 1, 1, 1);
	render().rasterizerState().cullMode = CullModes::CCW;
	auxRenderer()->drawSolidBox(55, 55, 55);
	auxRenderer()->drawSolidEllipsoid(50, 50, 50, 25, 25);
	auxRenderer()->drawAxis(1000);
	Matrix m;
	auxRenderer()->set2dProjection(0, 0, 1024, 768);
	auxRenderer()->draw2dTexturedQuad(1328338884, 0, 0, 77, 75);
	//m.setTranslation(input().mouse()->xPosition(), input().mouse()->yPosition(), 0);
	//render().setWorldMatrix(m);
	//debugRender()->setColor(1, 1, 0, 1);
	//debugRender()->draw3dCross(10);
	auxRenderer()->end();
}

void Editor::pushHistoryAction(EditorHistoryAction* pAction)
{
	if (!m_bEnableHistory)
	{
		return;
	}

	m_history.appendUnique(pAction);
}

void Editor::disableHistory()
{
	m_bEnableHistory = false;
}

void Editor::enableHistory()
{
	m_bEnableHistory = true;
}

bool Editor::isHistoryEnabled() const
{
	return m_bEnableHistory;
}

bool Editor::undo(u32 aSteps)
{
	return true;
}

bool Editor::redo(u32 aSteps)
{
	return true;
}

size_t Editor::historyActionCount() const
{
	return m_history.size();
}

EditorHistoryAction* Editor::historyActionAt(size_t aIndex) const
{
	return m_history[aIndex];
}

void Editor::clearHistory()
{
	for(size_t i = 0, iCount = m_history.size(); i < iCount; ++i)
	{
		delete m_history[i];
	}

	m_history.clear();
}

EditorCommand* Editor::findCommand(const String& aCmdName)
{
	auto iter = m_commands.find(aCmdName);

	if (iter != m_commands.end())
	{
		return iter->value;
	}

	return nullptr;
}

bool Editor::registerCommand(EditorCommand* pCmd)
{
	B_ASSERT(pCmd);
	m_commands[pCmd->name()] = pCmd;

	return true;
}

bool Editor::unregisterCommand(EditorCommand* pCmd)
{
	B_ASSERT(pCmd);
	auto iter = m_commands.begin();

	if (iter != m_commands.end())
	{
		delete pCmd;
		m_commands.erase(iter);
		return true;
	}

	return false;
}

bool Editor::isCommandRegistered(const String& aCmdName)
{
	return m_commands.find(aCmdName) != m_commands.end();
}

bool Editor::registerEvent(EditorEvent* pEvent)
{
	m_events.appendUnique(pEvent);
	return true;
}

bool Editor::unregisterEvent(EditorEvent* pEvent)
{
	auto iter = m_events.find(pEvent);
	
	if (iter == m_events.end())
	{
		return false;
	}
	
	m_events.erase(iter);
	
	return true;
}

bool Editor::isEventRegistered(const String& aEventName)
{
	auto iter = m_events.begin();
	
	while (iter != m_events.end())
	{
		if ((*iter)->name() == aEventName)
		{
			return true;
		}

		++iter;
	}
	
	return false;
}

bool Editor::triggerEvent(EditorEvent* pEvent, void* pUserData)
{
	auto iter = m_events.find(pEvent);
	
	if (iter == m_events.end())
	{
		return false;
	}
	
	// call the sinks before
	callEventSinks(pEvent, TriggerContexts::Before, pUserData);
	
	// call the event
	(*iter)->onTrigger(pUserData);

	// call the sinks after
	callEventSinks(pEvent, TriggerContexts::After, pUserData);

	return true;
}

bool Editor::triggerEvent(const String& aEventName, void* pUserData)
{
	EditorEvent* pEvent = nullptr;

	for (size_t i = 0; i < m_events.size(); ++i)
	{
		if (m_events[i]->name() == aEventName)
		{
			pEvent = m_events[i];
			break;
		}
	}

	if (!pEvent)
	{
		return false;
	}
	
	return triggerEvent(pEvent, pUserData);
}

void Editor::callEventSinks(EditorEvent* pEvent, TriggerContext aContext, void* pUserData)
{
	auto iter = m_eventSinks.begin();
	
	while (iter != m_eventSinks.end())
	{
		(*iter)->onEvent(pEvent, aContext, pUserData);
		++iter;
	}
}

bool Editor::registerEventSink(EditorEventSink* pEventSink)
{
	m_eventSinks.appendUnique(pEventSink);
	return true;
}

bool Editor::unregisterEventSink(EditorEventSink* pEventSink)
{
	auto iter = m_eventSinks.find(pEventSink);
	
	if (iter == m_eventSinks.end())
	{
		return false;
	}
	
	m_eventSinks.erase(iter);
	return true;
}

bool Editor::call(const String& aCommandName, CmdArgs* pArgs, CmdArgs* pReturnValues)
{
	auto iter = m_commands.find(aCommandName);
	
	if (iter != m_commands.end())
	{
		return iter->value->commandFunction()(pArgs, pReturnValues);
	}

	return false;
}

void Editor::onWindowEvent(OsWindow* pWnd)
{
	switch (render().osWindow()->eventType())
	{
	case WindowEvents::DoubleClick:
	case WindowEvent::MouseDown:
	case WindowEvent::MouseUp:
		{
			render().osWindow()->focus();
			break;
		}
	case WindowEvent::WindowResized:
		{
			render().resetViewport();
			break;
		}
	}
}

}