#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/string.h"

namespace engine
{
class E_API EditorHistoryAction
{
public:
	virtual ~EditorHistoryAction(){}
	const String& name() const
	{
		return m_name;
	}

	const String& description() const
	{
		return m_description;
	}

	virtual bool canUndo() const
	{
		return true;
	}

	virtual bool undo() = 0;
	virtual bool redo() = 0;

protected:
	String m_name;
	String m_description;
};

}