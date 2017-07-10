#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/string.h"
#include "defines.h"

namespace editor
{
struct EDITOR_API EditorHistoryAction
{
	String name;
	String description;

	virtual ~EditorHistoryAction(){}
	virtual bool undo() = 0;
	virtual bool redo() = 0;
};

}