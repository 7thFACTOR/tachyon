#include "base/device_input.h"
#include "base/window.h"
#include "base/logger.h"

namespace base
{
bool InputQueue::peekEvent(size_t index, InputEvent& event)
{
	if (index >= events.size())
	{
		return false;
	}
	
	event = events[index];
	
	return true;
}

bool InputQueue::popEvent(InputEvent& event)
{
	return events.popFirst(event);
}

void InputQueue::addEvent(const InputEvent& event)
{
	events.append(event);
}

void InputQueue::clearEvents()
{
	events.clear();
}

bool InputQueue::hasKeyDownEvent(InputKey key)
{
	for (auto& ev : events)
	{
		if (ev.type == InputEvent::Type::Key
			&& ev.key.code == key
			&& ev.key.down)
		{
			return true;
		}
	}
	
	return false;
}

bool InputQueue::hasKeyUpEvent(InputKey key)
{
	for (auto& ev : events)
	{
		if (ev.type == InputEvent::Type::Key
			&&ev.key.code == key
			&& !ev.key.down)
		{
			return true;
		}
	}

	return false;
}

bool InputQueue::hasMouseButtonDown(MouseButton button)
{
	for (auto& ev : events)
	{
		if (ev.type == InputEvent::Type::MouseDown
			&& ev.mouse.button == button)
		{
			return true;
		}
	}

	return false;
}

bool InputQueue::hasMouseButtonUp(MouseButton button)
{
	for (auto& ev : events)
	{
		if (ev.type == InputEvent::Type::MouseUp
			&& ev.mouse.button == button)
		{
			return true;
		}
	}

	return false;
}

}