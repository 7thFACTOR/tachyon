// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "base/string.h"
#include "base/dictionary.h"
#include "base/array.h"
#include "base/logger.h"

namespace base
{
//! used to store parsed arguments
//! arg name - the argument name
//! arg value - argument value or "switch" - means this is a switch with no value, or "*" - means this is a free text
typedef Array<KeyValuePair<String/*arg name*/, String/*arg value*/>> Arguments;

//! A command line argument parser
//! Example: some.exe "some path or text" /some1=121 /some2="haha" "other free text or path" -some -someOther -some=YES
class B_API CommandLineArguments
{
public:
	CommandLineArguments(int argc, char** argv);
	CommandLineArguments(const String& cmdLine, bool hasExeFileName = false);
	CommandLineArguments();
	void parse(const String& cmdLine, bool hasExeFileName = false);
	void parse(int argc, char** argv);
	//! \return the Nth free text argument from command line, or "" if no free text argument found
	//! \param freeTextIndex the index of the free text argument
	String getFreeText(u32 freeTextIndex = 0) const;
	//! \return switch value (Ex: /some=value (if ARGS_PARSER_ALLOW_SLASH_ARGS is defined) or -some=value or --some=value
	//! \param name switch name
	//! \param defaultValue the default string value to be returned if switch not found
	String getArgValue(const String& name, const String& defaultValue = "") const;
	//! \return true if the switch is specified in the command line
	//! \param name switch name (without / (if ARGS_PARSER_ALLOW_SLASH_ARGS is defined) ,- or -- in front)
	//! \param enabled default value to return if not found
	bool hasSwitch(const String& name, bool enabled = false) const;
	//! \return true if the switch is specified in the command line
	//! \param name switch name (without / (if ARGS_PARSER_ALLOW_SLASH_ARGS is defined) ,- or -- in front)
	//! \param enabled default value to return if not found
	bool hasArg(const String& name) const;
	bool hasArgs() const;
	void addArg(const String& name, const String& value, bool overwrite = false);
	void debug() const;

	Arguments arguments;
};

}