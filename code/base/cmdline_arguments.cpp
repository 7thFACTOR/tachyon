// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/cmdline_arguments.h"
#include "base/logger.h"

namespace base
{
#define ARGS_PARSER_ALLOW_SLASH_ARGS

CommandLineArguments::CommandLineArguments(int argc, char** argv)
{
	parse(argc, argv);
}

CommandLineArguments::CommandLineArguments(const String& cmdLine, bool hasExeFileName)
{
	parse(cmdLine, hasExeFileName);
}

CommandLineArguments::CommandLineArguments()
{}

void CommandLineArguments::parse(const String& cmdLine, bool hasExeFileName)
{
	const i32 maxArgsAllowed = 1024;
	char* argsArray[maxArgsAllowed];
	char** args = 0;
	char* crtPos = (char*)cmdLine.c_str();
	bool inStr = false;
	bool gotArg = false;
	i32 numArgs = 0;
		
	while (*crtPos)
	{
		if (*crtPos <= ' ')
		{
			++crtPos;
			gotArg = false;
			continue;
		}
			
		if (!gotArg)
		{
			gotArg = true;
			argsArray[numArgs++] = crtPos;
		}

		++crtPos;
	}

	if (hasExeFileName)
	{
		--numArgs;
		args = &argsArray[1];
	}
	else
	{
		args = &argsArray[0];
	}

	parse(numArgs, args);
}

void CommandLineArguments::parse(int argc, char** argv)
{
	String str, strName, strValue;

	arguments.clear();

	// start from 1, skip filename
	for (u32 i = 1; i < argc; ++i)
	{
		str = argv[i];

		if (str.length() > 1)
		{
#ifdef ARGS_PARSER_ALLOW_SLASH_ARGS
	#define ARGS_PARSER_CHECK_SLASH str[0] == '/'||
#else
	#define ARGS_PARSER_CHECK_SLASH
#endif
			if (ARGS_PARSER_CHECK_SLASH str[0] == '-')
			{
				i32 pos = str.findChar('=');
				i32 offset = (str[1] == '-') ? 2 : 0;
#ifdef ARGS_PARSER_ALLOW_SLASH_ARGS
				if (str[0] == '/')
					offset = 1;
#endif
				if (pos != String::noIndex)
				{
					strName = str.subString(offset, pos - offset);
					strValue = str.subString(pos + 1, str.length() - (pos + 1));
					arguments.append(KeyValuePair<String, String>(strName, strValue));
				}
				else
				{
					strName = str.subString(offset, str.length() - offset);
					strValue = "switch";
					arguments.append(KeyValuePair<String, String>(strName, strValue));
				}
			}
			else
			{
				arguments.append(KeyValuePair<String, String>("*", str));
			}
		}
	}
#undef ARGS_PARSER_CHECK_SLASH
}

String CommandLineArguments::getFreeText(u32 freeTextIndex) const
{
	auto iter = arguments.begin();
	int n = 0;

	while (iter != arguments.end())
	{
		if (iter->key == "*")
		{
			if (n == freeTextIndex)
				return iter->value;
			++n;
		}

		++iter;
	}

	return "";
}

String CommandLineArguments::getArgValue(const String& name, const String& defaultValue) const
{
	auto iter = arguments.begin();

	while (iter != arguments.end())
	{
		if (iter->key == name)
		{
			return iter->value;
		}

		++iter;
	}

	return defaultValue;
}

bool CommandLineArguments::hasSwitch(const String& name, bool enabled) const
{
	auto iter = arguments.begin();

	while (iter != arguments.end())
	{
		if (iter->key == name 
			&& iter->value == "switch"
			|| iter->value == "")
		{
			return true;
		}

		++iter;
	}

	return enabled;
}

bool CommandLineArguments::hasArg(const String& name) const
{
	auto iter = arguments.begin();

	while (iter != arguments.end())
	{
		if (iter->key == name)
		{
			return true;
		}

		++iter;
	}

	return false;
}

bool CommandLineArguments::hasArgs() const
{
	return !arguments.isEmpty();
}

void CommandLineArguments::debug() const

{
	auto iter = arguments.begin();

	while (iter != arguments.end())
	{
		B_LOG_INFO("'" << iter->key << "' is '" << iter->value << "'\n");
		++iter;
	}
}

void CommandLineArguments::addArg(const String& name, const String& value, bool overwrite)
{
	auto iter = arguments.begin();

	while (iter != arguments.end())
	{
		if (iter->key == name)
		{
			iter->value = value;
			return;
		}

		++iter;
	}

	arguments.append(KeyValuePair<String, String>(name, value));
}

}