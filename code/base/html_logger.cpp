// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <stdarg.h>
#include <stdio.h>
#include "base/html_logger.h"
#include "base/mutex.h"
#include "base/util.h"

namespace base
{
HtmlLogger::HtmlLogger(const String& filename, bool append)
	: Logger()
{
	flags = LogFlags::All;
	fileHandle = nullptr;
	lineCount = 0;

	if (filename.notEmpty())
	{
		open(filename, append);
	}
}

HtmlLogger::~HtmlLogger()
{}

void HtmlLogger::write(u32 channelIndex, LogItemType type, const String& module, u32 codeLine, const String& message)
{
	if (channelInfo[channelIndex].active)
	{
		DateTime dt;
		String typeStr;
		String msgColor;
		String dateTime, moduleStr, typeName, all, lineNumber, threadId;

		dt = getSystemDateTime();

		switch (type)
		{
		case LogItemType::Error:
			typeStr = "<b><span class='error'>ERROR<span></b>";
			msgColor = "error_msg";
			break;
		case LogItemType::Warning:
			typeStr = "<b><span class='warning'>WARNING<span></b>";
			msgColor = "warning_msg";
			break;
		case LogItemType::Info:
			typeStr = "<b><span class='info'>INFO<span></b>";
			msgColor = "info_msg";
			break;
		case LogItemType::Success:
			typeStr = "<b><span class='success'>SUCCESS<span></b>";
			msgColor = "success_msg";
			break;
		case LogItemType::Debug:
			typeStr = "<b><span class='debug'>DEBUG<span></b>";
			msgColor = "debug_msg";
			break;
		default:
			typeStr = "?";
			break;
		}

		if (!!(flags & LogFlags::DateTime))
		{
			dateTime <<
				"<td width='100' style='font-size: 10px'>"
				<< dt.hour << ":" << dt.minute << ":" << dt.second << ":" << dt.millisecond
				<< dt.day << dt.month << dt.year;
		}

		if (!!(flags & LogFlags::Module))
		{
			moduleStr << "<td width='100' style='color: #FFCC99'>" << module << " @" << codeLine << "</td>";
		}

		if (!!(flags & LogFlags::Type))
		{
			typeName << "<td class='logtype' width='50' style='font-size: 10px'>" << typeStr << "</td>";
		}

		if (!!(flags & LogFlags::LineNumber))
		{
			lineNumber << "<td>" << String::intDecimals(6) << (lineCount++) << "</td>";
		}

		if (!!(flags & LogFlags::ThreadId))
		{
			threadId << "<td>" << (u64)getCurrentThreadId() << "</td>";
		}

		String msg;

		msg = message;
		msg.replace("<", "&lt;");
		msg.replace(">", "&gt;");
		msg.replace(" ", "&nbsp;");

		all << "<tr>"
			<< lineNumber << dateTime << typeName << threadId
			<< "<td class='" << msgColor << "'>" << msg << "</td>" << moduleStr << "</tr>\n";
		fwrite(all.c_str(), all.getByteSize(), 1, (FILE*)fileHandle);
		fflush((FILE*)fileHandle);
	}

	for (auto logger : childLoggers)
	{
		logger->write(channelIndex, type, module, codeLine, message);
	}
}

bool HtmlLogger::open(const String& filename, bool append)
{
	close();
	String openMode = append ? "a+" : "w";

	fileHandle = utf8fopen(filename, openMode);

	if (fileHandle)
	{
		String txt = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"
			"<html>"
			"<head>"
			"<title>Engine Log</title>"
			"<style>"
			"html,body"
			"{"
			"	font-family: verdana;"
			"	color: white;"
			"	font-size: 12px;"
			"	background: #383838;"
			"}"
			""
			".error"
			"{"
			"	color: red;"
			"}"
			""
			".warning"
			"{"
			"	color: #E48901;"
			"}"
			""
			".info"
			"{"
			"	color: #66FFFF;"
			"}"
			""
			".success"
			"{"
			"	color: #99FF33;"
			"}"
			""
			".debug"
			"{"
			"	color: #CC99FF;"
			"}"
			".error_msg"
			"{"
			"	color: yellow;"
			"	background: #990000;"
			"}"
			""
			".warning_msg"
			"{"
			"	color: #E48901;"
			"}"
			""
			".info_msg"
			"{"
			"	color: #66FFFF;"
			"}"
			""
			".success_msg"
			"{"
			"	color: #99FF33;"
			"}"
			""
			".debug_msg"
			"{"
			"	color: #CC99FF;"
			"}"
			"td"
			"{"
			"	background: #484848;"
			"	font-size: 11px;"
			"}"
			"td.logtype"
			"{"
			"	background: #333333;"
			"	text-align: center;"
			"}"
			"th"
			"{"
			"	background: black;"
			"	text-align: center;"
			"	font-size: 11px;"
			"	color: #9A9A9A;"
			"}"
			""
			"</style>"
			"</head>"
			"<body>"
			"<div style='font-size: 25px; font-family: arial narrow'>Engine Log</div>"
			"<table cellspacing=1 cellpadding=2>";

		fwrite(txt.c_str(), txt.getByteSize(), 1, (FILE*)fileHandle);

		String cols;

		if (!!(flags & LogFlags::LineNumber))
		{
			cols += "<th>#</th>";
		}

		if (!!(flags & LogFlags::DateTime))
		{
			cols += "<th>Time/Date</th>";
		}

		if (!!(flags & LogFlags::Type))
		{
			cols += "<th>Type</th>";
		}

		if (!!(flags & LogFlags::ThreadId))
		{
			cols += "<th>Thread Id</th>";
		}

		cols += "<th>Message</th>";

		if (!!(flags & LogFlags::Module))
		{
			cols += "<th>Module</th>";
		}

		fwrite(cols.c_str(), cols.getByteSize(), 1, (FILE*)fileHandle);
	}

	return (fileHandle != nullptr);
}

bool HtmlLogger::close()
{
	if (fileHandle)
	{
		String txt = "</table></body></html>";
		fwrite(txt.c_str(), txt.getByteSize(), 1, (FILE*)fileHandle);
		fclose((FILE*)fileHandle);
	}

	return true;
}

}