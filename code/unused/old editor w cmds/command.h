#pragma once
#include "base/types.h"
#include "base/array.h"
#include "core/defines.h"
#include <string.h>
#include <stdio.h>
#include "base/string.h"
#include "base/class_registry.h"

namespace engine
{
using namespace base;

struct CmdArgs
{
	Array<String> values;

	const String& argAt(size_t aIndex) const
	{
		return values[aIndex];
	}

	CmdArgs& operator << (i32 val)
	{
		char valstr[32];
		sprintf(valstr, "%d", val);
		values.append(valstr);

		return *this;
	}

	CmdArgs& operator << (f32 val)
	{
		char valstr[32];
		sprintf(valstr, "%f", val);
		values.append(valstr);

		return *this;
	}

	CmdArgs& operator << (double val)
	{
		char valstr[32];
		sprintf(valstr, "%g", val);
		values.append(valstr);

		return *this;
	}

	CmdArgs& operator << (void* val)
	{
		char valstr[32];
		sprintf(valstr, "%p", val);
		values.append(valstr);

		return *this;
	}

	CmdArgs& operator << (const char* val)
	{
		values.append(val);

		return *this;
	}

	CmdArgs& operator << (const String& val)
	{
		values.append(val);

		return *this;
	}

	CmdArgs& operator << (char val)
	{
		char valstr[6];
		sprintf(valstr, "%d", val);
		values.append(valstr);

		return *this;
	}

	CmdArgs& operator << (bool val)
	{
		char valstr[6];
		sprintf(valstr, "%d", (int)val);
		values.append(valstr);

		return *this;
	}
};

typedef bool (*TPfnEditorCommand)(CmdArgs* pArgs, CmdArgs* pReturnValues);

class E_API EditorCommand : public ClassInfo
{
public:
	virtual ~EditorCommand(){}
	virtual const char* name() const = 0;
	virtual TPfnEditorCommand commandFunction() = 0;
};

#define E_BEGIN_EDITOR_COMMAND(cmdGroup, cmdName) \
	namespace ns_##cmdGroup##_##cmdName \
	{ \
	bool cmdGroup##_##cmdName##_EditorCommandFunc(CmdArgs* pArgs, CmdArgs* pReturnValues); \
	class cmdGroup##_##cmdName##_EditorCommand: public EditorCommand \
	{ public:	B_DECLARE_DERIVED_CLASS(cmdGroup##_##cmdName##_EditorCommand, EditorCommand); \
	const char* name() const { return #cmdGroup"."#cmdName; } \
	TPfnEditorCommand commandFunction() { return cmdGroup##_##cmdName##_EditorCommandFunc; } }; \
	B_REGISTER_CLASS(cmdGroup##_##cmdName##_EditorCommand); \
	bool cmdGroup##_##cmdName##_EditorCommandFunc(CmdArgs* pArgs, CmdArgs* pReturnValues){ bool bCmdReturn = true;

#define E_END_EDITOR_COMMAND return bCmdReturn; } }

}