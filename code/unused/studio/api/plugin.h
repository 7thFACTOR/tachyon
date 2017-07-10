#pragma once
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>

class QMainWindow;
struct IEditorPlugin;
struct IEditor;

typedef IEditorPlugin* (*TPfnCreatePluginInstance)(IEditor* pEditor);

#define NSTUDIO_BEGIN_PLUGIN \
	IEditor* ed(nullptr); \
	struct NStudioPlugin: IEditorPlugin \
	{ \
	NStudioPlugin(IEditor* pEditor) \
			{ ed = pEditor; }

#define NSTUDIO_END_PLUGIN }; \
	extern "C" \
	{ \
		__declspec(dllexport) IEditorPlugin* createPluginInstance(IEditor* pEditor) \
		{ \
			return new NStudioPlugin(pEditor); \
		} \
	};

struct CmdArgs
{
	typedef std::vector<std::string> TCommandArgValues;

	std::string toString()
	{
		std::string str;

		for (int i = 0; i < values.size(); ++i)
		{
			str += values[i];

			if (values.size() - 1 != i)
			{
				str += ",";
			}
		}

		return str;
	}

	CmdArgs& operator << (int val)
	{
		char valstr[32];
		sprintf(valstr, "%d", val);
		values.push_back(valstr);

		return *this;
	}

	CmdArgs& operator << (float val)
	{
		char valstr[32];
		sprintf(valstr, "%f", val);
		values.push_back(valstr);

		return *this;
	}

	CmdArgs& operator << (double val)
	{
		char valstr[32];
		sprintf(valstr, "%g", val);
		values.push_back(valstr);

		return *this;
	}

	CmdArgs& operator << (void* val)
	{
		char valstr[32];
		sprintf(valstr, "%p", val);
		values.push_back(valstr);

		return *this;
	}

	CmdArgs& operator << (const char* val)
	{
		values.push_back(val);

		return *this;
	}

	CmdArgs& operator << (char val)
	{
		char valstr[6];
		sprintf(valstr, "%d", val);
		values.push_back(valstr);

		return *this;
	}

	CmdArgs& operator << (bool val)
	{
		char valstr[6];
		sprintf(valstr, "%d", (int)val);
		values.push_back(valstr);

		return *this;
	}

	TCommandArgValues values;
};


struct IEditorPlugin
{
	virtual ~IEditorPlugin(){}
	virtual bool onLoad() = 0;
	virtual bool onUnload() = 0;
};

struct IEditor
{
	virtual ~IEditor(){}
	virtual QMainWindow* mainWindow() = 0;
	virtual bool call(const char* pCmdName, CmdArgs& rArgs = CmdArgs(), CmdArgs& rReturnValues = CmdArgs()) = 0;
};