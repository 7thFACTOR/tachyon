// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "defines.h"
#include "base/types.h"
#include "base/defines.h"
#include "core/defines.h"
#include "event.h"
#include "input/window.h"
#include "base/dictionary.h"
#include "logic/types.h"
#include "logic/component.h"
#include "logic/entity.h"
#include "graphics/shape_renderer.h"
#include "component_editor_proxy.h"
#include "logic/component_updaters/render/render_component_updater.h"

namespace editor
{
using namespace base;
using namespace engine;

class EditorEventSink;
class EditorEvent;
class EditorHistoryAction;

class EDITOR_API Editor : public RenderObserver
{
public:
	typedef Array<EditorEvent*> EditorEvents;
	typedef Array<EditorEventSink*> EditorEventSinks;

	Editor();
	virtual ~Editor();
	void initialize();
	void shutdown();
	void update();
	void renderGizmos();
	bool registerEvent(EditorEvent* pEvent);
	bool unregisterEvent(EditorEvent* pEvent);
	bool isEventRegistered(const String& eventName);
	void registerComponentEditorProxy(ComponentEditorProxy* proxy);
	void unregisterComponentEditorProxy(ComponentEditorProxy* proxy);
	bool triggerEvent(EditorEvent* pEvent, void* pUserData);
	bool triggerEvent(const String& eventName, void* pUserData);
	//! this method is used by events to call event sinks during the event execution
	void callEventSinks(EditorEvent* pEvent, TriggerContext context, void* pUserData);
	bool registerEventSink(EditorEventSink* pEventSink);
	bool unregisterEventSink(EditorEventSink* pEventSink);
	void pushHistoryAction(EditorHistoryAction* pAction);
	void setEnableHistory(bool value);
	bool isHistoryEnabled() const;
	size_t historyActionCount() const;
	EditorHistoryAction* historyActionAt(size_t index) const;
	void clearHistory();
	bool undo(u32 steps = 1);
	bool redo(u32 steps = 1);
	const Array<Entity*>& getSelectedEntities() { return selectedEntities; }
	const Array<Component*>& getSelectedComponents() { return selectedComponents; }
	
protected:
	void onAfterWorldRender() override;

	Array<EditorHistoryAction*> history;
	Array<Entity*> selectedEntities;
	Array<Component*> selectedComponents;
	Dictionary<ComponentTypeId, ComponentEditorProxy*> componentEditorProxies;
	EditorEvents events;
	EditorEventSinks eventSinks;
	bool enableHistory = true;
	ShapeRenderer* shapeRenderer = nullptr;
	World* mainWorld;
};

}