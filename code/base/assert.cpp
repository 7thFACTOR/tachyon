// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/assert.h"
#include "base/platform.h"
#include "base/util.h"

namespace base
{
bool assertMessage(
	const String& filename,
	const String& function,
	u32 line,
	const String& expression,
	const String& message)
{
	String str;

	if (message.length())
	{
		str << "Message: " << message << "\n\n";
	}

	if (MessageBoxResult::Yes == 
		showMessageBox(MessageBoxFlags::YesNo | MessageBoxFlags::Error,
		String(B_TEXT("Assertion failed in file: '")) << filename << "'\n\nFunction: '" << function << "'\nCode line: "<< line << "\n\nUnsatisfied condition: " << expression << "\n\n" << (message.length() ? str : B_TEXT("")) << "Click 'Yes' to debug the code, or 'No' to continue."))
	{
		return true;
	}
	else
	{
		return false;
	}
}

}