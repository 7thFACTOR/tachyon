#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "core/defines.h"
#include "editor/event.h"
#include "editor/command.h"
#include "base/os_window.h"
#include "base/map.h"

namespace engine
{
using namespace base;

class EditorEventSink;
class EditorEvent;
class EditorHistoryAction;
class EditorCommand;
class EditorServer;

class E_API Editor : public ClassInfo, public IWindowObserver
{
public:
	B_DECLARE_CLASS(Editor);
	typedef Map<String, EditorCommand*> TCommands;
	typedef Array<EditorEvent*> TEvents;
	typedef Array<EditorEventSink*> TEventSinks;

	Editor();
	virtual ~Editor();
	void initialize();
	void shutdown();
	void update();
	void renderGizmos();
	bool registerCommand(EditorCommand* pCmd);
	bool unregisterCommand(EditorCommand* pCmd);
	bool isCommandRegistered(const String& aCmdName);
	EditorCommand* findCommand(const String& aCmdName);
	bool registerEvent(EditorEvent* pEvent);
	bool unregisterEvent(EditorEvent* pEvent);
	bool isEventRegistered(const String& aEventName);
	bool triggerEvent(EditorEvent* pEvent, void* pUserData);
	bool triggerEvent(const String& aEventName, void* pUserData);
	//! this method is used by events to call event sinks during the event execution
	void callEventSinks(EditorEvent* pEvent, TriggerContext aContext, void* pUserData);
	bool registerEventSink(EditorEventSink* pEventSink);
	bool unregisterEventSink(EditorEventSink* pEventSink);
	bool call(const String& aCommandName, CmdArgs* pArgs = nullptr, CmdArgs* pReturnValues = nullptr);
	void pushHistoryAction(EditorHistoryAction* pAction);
	void disableHistory();
	void enableHistory();
	bool isHistoryEnabled() const;
	size_t historyActionCount() const;
	EditorHistoryAction* historyActionAt(size_t aIndex) const;
	void clearHistory();
	bool undo(u32 aSteps = 1);
	bool redo(u32 aSteps = 1);

	void onWindowEvent(OsWindow* pWnd);

protected:
	TCommands m_commands;
	Array<EditorHistoryAction*> m_history;
	TEvents m_events;
	TEventSinks m_eventSinks;
	bool m_bEnableHistory;
	EditorServer* m_pServer;
};

}