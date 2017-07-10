#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "core/defines.h"

namespace engine
{
/// An event is triggered when certain actions are happening in the editor. For example we can have an event at Save  	level or on Export terrain, or object moved by mouse in the scene.
class E_API EditorEvent
{
public:
	virtual ~EditorEvent(){}
	virtual const char* name() = 0;
	virtual void onTrigger(void* pUserData) = 0;
	virtual void* userData() = 0;
};

/// When the event sink call is received, before, during or after the event was consumed
/// The eTriggerContext_During can be used to have lengthy events being processed and many triggered to update some progress bars
namespace TriggerContexts
{
	enum TriggerContext
	{
		Before,
		During,
		After
	};
}
typedef TriggerContexts::TriggerContext TriggerContext;

/// The user can register its event sink so it receives notifications about events happening in the editor ecosystem,
// coming from other modules or the editor core itself
class E_API EditorEventSink
{
public:
	virtual ~EditorEventSink(){}
	virtual void onEvent(EditorEvent* pEvent, TriggerContext aContext, void* pUserData) = 0;
};

}