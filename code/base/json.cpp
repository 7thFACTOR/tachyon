// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale>
#include "base/json.h"
#include "base/file.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/util.h"
#include "base/defines.h"
#include "base/types.h"

namespace base
{
static JsonValue dummyValue;
static JsonObject dummyObject;
static JsonArray dummyArray;
static String dummyString;

#define JSON_LOG_ERROR(json, expecting) setError(json, __LINE__, expecting)

bool jsonIsIdentifierChar(char chr)
{
	return ((chr >= 'A' && chr <= 'Z')
		|| (chr >= 'a' && chr <= 'z')
		|| (chr >= '0' && chr <= '9')
		|| chr == '_'
		|| chr == '@'
		|| chr == '#'
		|| chr == '$');
}

bool jsonIsWhiteSpace(char chr, bool eolIsWhitespace = true)
{
	if (!eolIsWhitespace)
	{
		return chr == ' ' || chr == '\t';
	}

	return chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r';
}

char* jsonSkipWhitespace(char* text, u32& crtLine, bool eolIsWhitespace = true)
{
	if (!text)
	{
		return nullptr;
	}

	while (*text && jsonIsWhiteSpace(*text, eolIsWhitespace))
	{
		if (*text == '\n')
		{
			++crtLine;
		}

		++text;
	}

	return text;
}

char* jsonSkipToNextLine(char* text, u32& crtLine)
{
	if (!text)
	{
		return nullptr;
	}

	while (*text)
	{
		if (*text == '\n')
		{
			++crtLine;
			return text;
		}

		++text;
	}

	return text;
}

char* jsonReadIdentifier(char* text, String& token)
{
	if (!text)
	{
		return nullptr;
	}

	token = "";

	while (*text && jsonIsIdentifierChar(*text))
	{
		token += *text;
		++text;
	}

	return text;
}

char* jsonReadNonStringValue(char* text, String& token)
{
	if (!text)
	{
		return nullptr;
	}

	token = "";

	while (*text
		&& !jsonIsWhiteSpace(*text)
		&& *text != ',')
	{
		token += *text;
		++text;
	}

	return text;
}

char* jsonReadNumber(char* text, String& token, bool& isFloat, bool& error)
{
	if (!text)
	{
		return nullptr;
	}

	isFloat = false;
	error = false;
	char* head = text;

	while (*text 
		&& !jsonIsWhiteSpace(*text)
		&& (isdigit(*text) || *text == '-' || *text == '.' || *text == 'e' || *text == 'E'))
	{
		if (*text == '-' && text != head)
		{
			error = true;
			return text;
		}

		if (*text == '.' && !isFloat)
		{
			isFloat = true;
		}

		token += *text;
		++text;
	}

	if (!*text)
	{
		if (!jsonIsWhiteSpace(*text))
		{
			error = true;
		}
	}

	return text;
}

bool jsonIsIntStr(char* text)
{
	if (!text)
	{
		return false;
	}

	if (!strcmp(text, ""))
	{
		return false;
	}

	char* head = text;

	while (*text 
		&& !jsonIsWhiteSpace(*text)
		&& (isdigit(*text) || *text == '-'  || *text == '.'))
	{
		if (*text == '-' && text != head)
		{
			return false;
		}

		if (*text == '.')
		{
			return false;
		}

		++text;
	}

	if (*text)
	{
		if (!jsonIsWhiteSpace(*text))
		{
			return false;
		}
	}

	return true;
}

bool jsonIsFloatStr(char* text)
{
	if (!text)
	{
		return false;
	}

	if (!strcmp(text, ""))
	{
		return false;
	}

	char* head = text;

	while (*text 
		&& !jsonIsWhiteSpace(*text)
		&& (isdigit(*text) || *text == '-'  || *text == '.' || *text == 'e' || *text == 'E'))
	{
		++text;
	}

	if (*text)
	{
		if (!jsonIsWhiteSpace(*text))
		{
			return false;
		}
	}
	
	return true;
}

char* jsonReadString(char* text, String& token)
{
	if (!text)
	{	
		return nullptr;
	}

	token = "";

	while (*text
		&& *text != '"')
	{
		// special case of \"
		if (*text == '\\')
		{
			if (*(text + 1) == '"')
			{
				// skip it
				++text;
			}
		}

		token += *text;
		++text;
	}

	if (*text == '"')
	{
		++text;
	}

	return text;
}

const JsonValue& JsonValue::operator = (const JsonValue& other)
{
	free();
	type = other.type;

	switch (type)
	{
	case JsonValueType::Integer:
		{
			value = new i64();
			*(i64*)value = other.asI64();
			break;
		}
	case JsonValueType::Float:
		{
			value = new f64();
			*(f64*)value = other.asF64();
			break;
		}
	case JsonValueType::Bool:
		{
			value = new bool();
			*(bool*)value = other.asBool();
			break;
		}
	case JsonValueType::String:
		{
			value = new String();
			*(String*)value = other.asString();
			break;
		}
	case JsonValueType::Object:
		{
			value = new JsonObject();
			*(JsonObject*)value = *other.asObject();
			break;
		}
	case JsonValueType::Array:
		{
			value = new JsonArray();
			*(JsonArray*)value = *other.asArray();
			break;
		}
	default:
		break;
	}

	return *this;
}

i32 JsonValue::asI32() const
{
	if (!value)
	{
		return 0;
	}

	B_ASSERT(type == JsonValueType::Integer || type == JsonValueType::Float);

	if (type == JsonValueType::Integer)
	{
		return *(i32*)value;
	}
	else if (type == JsonValueType::Float)
	{
		return (i32)(*(f64*)value);
	}

	return 0;
}

u32 JsonValue::asU32() const
{
	if (!value)
	{
		return 0;
	}

	B_ASSERT(type == JsonValueType::Integer || type == JsonValueType::Float);

	if (type == JsonValueType::Integer)
	{
		return *(u32*)value;
	}
	else if (type == JsonValueType::Float)
	{
		return (u32)(*(f64*)value);
	}

	return 0;
}

i64 JsonValue::asI64() const
{
	if (!value)
	{
		return 0;
	}

	B_ASSERT(type == JsonValueType::Integer || type == JsonValueType::Float);

	if (type == JsonValueType::Integer)
	{
		return *(i64*)value;
	}
	else if (type == JsonValueType::Float)
	{
		return (i64)(*(f64*)value);
	}

	return 0;
}

u64 JsonValue::asU64() const
{
	if (!value)
	{
		return 0;
	}

	B_ASSERT(type == JsonValueType::Integer || type == JsonValueType::Float);

	if (type == JsonValueType::Integer)
	{
		return *(u64*)value;
	}
	else if (type == JsonValueType::Float)
	{
		return (u64)(*(f64*)value);
	}

	return 0;
}

f32 JsonValue::asF32() const
{
	if (!value)
	{
		return 0.0f;
	}

	B_ASSERT(type == JsonValueType::Integer || type == JsonValueType::Float);

	if (type == JsonValueType::Integer)
	{
		return (f32)(*(i64*)value);
	}
	else if (type == JsonValueType::Float)
	{
		return (f32)(*(f64*)value);
	}

	return 0.0f;
}

f64 JsonValue::asF64() const
{
	if (!value)
	{
		return 0.0f;
	}

	B_ASSERT(type == JsonValueType::Integer || type == JsonValueType::Float);

	if (type == JsonValueType::Integer)
	{
		return (f64)(*(i64*)value);
	}
	else if (type == JsonValueType::Float)
	{
		return *(f64*)value;
	}

	return 0.0f;
}

bool JsonValue::asBool() const
{
	if (!value)
	{
		return false;
	}

	B_ASSERT(type == JsonValueType::Bool);

	return *(bool*)value;
}

const String& JsonValue::asString() const
{
	if (!value)
	{
		return dummyString;
	}

	B_ASSERT(type == JsonValueType::String);

	return *(String*)value;
}

void JsonValue::setValue(const String& newValue)
{
	if (type != JsonValueType::String)
	{
		free();
		value = new String();
		type = JsonValueType::String;
	}

	*(String*)value = newValue;
}

void JsonValue::setValue(i32 newValue)
{
	if (type != JsonValueType::Integer)
	{
		free();
		value = new i64();
		type = JsonValueType::Integer;
	}

	*(i64*)value = newValue;
}

void JsonValue::setValue(u32 newValue)
{
	if (type != JsonValueType::Integer)
	{
		free();
		value = new u64();
		type = JsonValueType::Integer;
	}

	*(u64*)value = newValue;
}


void JsonValue::setValue(i64 newValue)
{
	if (type != JsonValueType::Integer)
	{
		free();
		value = new i64();
		type = JsonValueType::Integer;
	}

	*(i64*)value = newValue;
}

void JsonValue::setValue(u64 newValue)
{
	if (type != JsonValueType::Integer)
	{
		free();
		value = new u64();
		type = JsonValueType::Integer;
	}

	*(u64*)value = newValue;
}

void JsonValue::setValue(f32 newValue)
{
	if (type != JsonValueType::Float)
	{
		free();
		value = new f64();
		type = JsonValueType::Float;
	}

	*(f64*)value = newValue;
}

void JsonValue::setValue(f64 newValue)
{
	if (type != JsonValueType::Float)
	{
		free();
		value = new f64();
		type = JsonValueType::Float;
	}

	*(f64*)value = newValue;
}

void JsonValue::setValue(bool newValue)
{
	if (type != JsonValueType::Bool)
	{
		free();
		value = new bool();
		type = JsonValueType::Bool;
	}

	*(bool*)value = newValue;
}

void JsonValue::setValue(JsonObject* newValue)
{
	if (type != JsonValueType::Object)
	{
		free();
		type = JsonValueType::Object;
	}

	value = newValue;
}

void JsonValue::setValue(JsonArray* newValue)
{
	if (type != JsonValueType::Array)
	{
		free();
		type = JsonValueType::Array;
	}

	value = newValue;
}

void JsonValue::assignValue(JsonValueType newType, void* newValue)
{
	type = newType;
	value = newValue;
}

void JsonValue::free()
{
	switch (type)
	{
	case JsonValueType::Integer:
		{
			i64* ptr = (i64*)value;
			B_SAFE_DELETE(ptr);
			break;
		}
	case JsonValueType::Float:
		{
			f64* ptr = (f64*)value;
			B_SAFE_DELETE(ptr);
			break;
		}
	case JsonValueType::Bool:
		{
			bool* ptr = (bool*)value;
			B_SAFE_DELETE(ptr);
			break;
		}
	case JsonValueType::String:
		{
			String* ptr = (String*)value;
			B_SAFE_DELETE(ptr);
			break;
		}
	case JsonValueType::Object:
		{
			JsonObject* ptr = (JsonObject*)value;
			B_SAFE_DELETE(ptr);
			break;
		}
	case JsonValueType::Array:
		{
			JsonArray* ptr = (JsonArray*)value;

			for (size_t i = 0; i < ptr->size(); i++)
			{
				delete ptr->at(i);
			}

			B_SAFE_DELETE(ptr);
			break;
		}
	default:
		break;
	}

	type = JsonValueType::Null;
	value = nullptr;
}

JsonObject* JsonValue::asObject() const
{
	if (type != JsonValueType::Object)
		return nullptr;

	return (JsonObject*)value;
}

JsonArray* JsonValue::asArray() const
{
	if (type != JsonValueType::Array)
		return nullptr;

	return (JsonArray*)value;
}

String JsonValue::toString() const
{
	String str;

	B_ASSERT(value);

	if (!value)
	{
		return "";
	}

	switch (type)
	{
	case JsonValueType::Integer:
		str << *(i64*)value;
		break;
	case JsonValueType::Float:
		str << String::floatDecimals(6) << *(f64*)value;
		break;
	case JsonValueType::Bool:
		str = *(bool*)value ? "true" : "false";
		break;
	case JsonValueType::String:
		str = *(String*)value;
		break;
	default:
		B_ASSERT(!"Cannot make a string out of an array or object");
		break;
	}

	return str;
}

//---

bool JsonObject::isEmpty() const
{
	return members.isEmpty();
}

JsonValue* JsonObject::getValueOf(const String& key) const
{
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i].key == key)
		{
			return members[i].value;
		}
	}

	return &dummyValue;
}

String JsonObject::getString(const String& name, const String& defaultValue) const
{
	if (!hasKey(name))
	{
		return defaultValue;
	}

	return getValueOf(name)->asString();
}

i32 JsonObject::getI32(const String& name, i32 defaultValue) const
{
	if (!hasKey(name))
	{
		return defaultValue;
	}

	return getValueOf(name)->asI32();
}

u32 JsonObject::getU32(const String& name, u32 defaultValue) const
{
	if (!hasKey(name))
	{
		return defaultValue;
	}

	return getValueOf(name)->asU32();
}

i64 JsonObject::getI64(const String& name, i64 defaultValue) const
{
	if (!hasKey(name))
	{
		return defaultValue;
	}

	return getValueOf(name)->asI64();
}

u64 JsonObject::getU64(const String& name, u64 defaultValue) const
{
	if (!hasKey(name))
	{
		return defaultValue;
	}

	return getValueOf(name)->asU64();
}

f32 JsonObject::getF32(const String& name, f32 defaultValue) const
{
	if (!hasKey(name))
	{
		return defaultValue;
	}

	return getValueOf(name)->asF32();
}

f64 JsonObject::getF64(const String& name, f64 defaultValue) const
{
	if (!hasKey(name))
	{
		return defaultValue;
	}

	return getValueOf(name)->asF64();
}

bool JsonObject::getBool(const String& name, bool defaultValue) const
{
	if (!hasKey(name))
	{
		return defaultValue;
	}

	return getValueOf(name)->asBool();
}

JsonObject* JsonObject::getObject(const String& name) const
{
	return getValueOf(name)->asObject();
}

JsonArray* JsonObject::getArray(const String& name) const
{
	return getValueOf(name)->asArray();
}

bool JsonObject::hasKey(const String& key) const
{
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i].key == key)
		{
			return true;
		}
	}

	return false;
}

const JsonObject::NameValuePairs& JsonObject::getMembers() const
{
	return members;
}

void JsonObject::free()
{
	auto iter = members.begin();

	while (iter != members.end())
	{
		if (iter->value)
			delete iter->value;
		++iter;
	}

	members.clear();
}

void JsonObject::setValue(const String& name, JsonValue* val)
{
	auto iter = members.begin();

	while (iter != members.end())
	{
		if (iter->key == name)
		{
			if (iter->value)
			{
				delete iter->value;
			}

			iter->value = val;
			return;
		}
		
		++iter;
	}

	members.append(KeyValuePair<String, JsonValue*>(name, val));
}

JsonValue* JsonObject::getValuePtr(const String& name)
{
	auto iter = members.begin();

	while (iter != members.end())
	{
		if (iter->key == name)
		{
			return iter->value;
		}

		++iter;
	}

	return &dummyValue;
}

JsonDocument::JsonDocument()
{
	currentLine = 0;
	gotErrors = false;
	currentWriteObject = nullptr;
	currentWriteArray = nullptr;
	currentWriteValueType = JsonValueType::Null;
}

JsonDocument::JsonDocument(const String& filename, bool doParse)
{
	loadTextOnly(filename);

	if (doParse)
	{
		parse();
	}
}

JsonDocument::~JsonDocument()
{
	free();
}

bool JsonDocument::loadTextOnly(const String& jsonFilename)
{
	filename = jsonFilename;
	json = "";

	File file;
		
	if (!file.open(jsonFilename, FileOpenFlags::BinaryRead))
	{
		return false;
	}

	file.readAllText(json);
	file.close();

	return true;
}

bool JsonDocument::loadAndParse(const String& jsonFilename)
{
	bool ok = loadTextOnly(jsonFilename);

	if (!ok)
	{
		return false;
	}

	return parse();
}

void JsonDocument::setJsonText(const String& text)
{
	json = text;
}

const String& JsonDocument::getJsonText() const
{
	return json;
}

bool JsonDocument::parse()
{
	currentLine = 1;
	errorText = "";
	gotErrors = false;

	if (json.isEmpty())
	{
		return true;
	}

	char* str = (char*)json.c_str();

	// skip spaces
	str = jsonSkipWhitespace(str, currentLine);
	
	if (!*str)
	{
		return true;
	}

	char expectEndChar = 0;

	// so we start an object ?
	if (*str == '{'
		|| *str == '"'
		|| jsonIsIdentifierChar(*str))
	{
		if (*str == '{')
		{
			++str;
			// expect closing bracket only if started
			expectEndChar = '}';
		}

		str = parseObject(str, &root);
	}
	else
	{
		JSON_LOG_ERROR(str, "'{', or key name");
		return false;
	}

	if (expectEndChar)
	{
		if (expectEndChar != *str)
		{
			JSON_LOG_ERROR(str, expectEndChar);
			return false;
		}

		++str;
	}

	// skip spaces till end of file
	str = jsonSkipWhitespace(str, currentLine);

	if (*str)
	{
		JSON_LOG_ERROR(str, "single root element");
		return false;
	}

	return true;
}

void JsonDocument::free()
{
	root.free();
}

char* JsonDocument::parseObject(char* str, JsonObject* parent)
{
	String name;
	String value;

	while (*str && !gotErrors)
	{
		// skip spaces
		str = jsonSkipWhitespace(str, currentLine);

		if (!*str)
		{
			break;
		}

		// we got a comment
		if (*str == '#')
		{
			str = jsonSkipToNextLine(str, currentLine);
		}

		str = jsonSkipWhitespace(str, currentLine);

		// we got a value name as: "name"
		if (*str == '"')
		{
			++str;
			// read name
			str = jsonReadString(str, name);

			if (name.isEmpty())
			{
				JSON_LOG_ERROR(str, "not empty value name");
				return str;
			}
		}
		// we got a value name as: name
		else if (jsonIsIdentifierChar(*str))
		{
			// read name
			str = jsonReadIdentifier(str, name);
		}

		// now skip until : or = or CR or { or [
		str = jsonSkipWhitespace(str, currentLine, false);

		// next, read value
		if (*str == ':' 
			|| *str == '=' 
			|| *str == '{' 
			|| *str == '[' 
			|| *str == '\n' 
			|| *str == '\r')
		{
			if (*str != '{' && *str != '[')
			{
				++str;
				str = jsonSkipWhitespace(str, currentLine);
			}

			JsonValue* pVal = new JsonValue();

			str = parseValue(str, pVal);
			parent->setValue(name, pVal);
		}
		else if (*str && *str != '}')
		{
			JSON_LOG_ERROR(str, "':' or '=' or <EOL>");
			return str;
		}

		// skip white spaces
		str = jsonSkipWhitespace(str, currentLine);

		// ended ?
		if (!*str)
		{
			return str;
		}

		// ended object ?
		if (*str == '}')
		{
			return str;
		}
		// other value next ?
		else if (*str == ',')
		{
			++str;
			continue;
		}
		// other value next ?
		else if (*str == '"' || jsonIsIdentifierChar(*str))
		{
			continue;
		}
		else if (*str)
		{
			JSON_LOG_ERROR(str, "'}', ',', '\"' or identifier name");
			return str;
		}
	}

	return str;
}

char* JsonDocument::parseValue(char* str, JsonValue* value)
{
	String strValue;

	if (*str && !gotErrors)
	{
		// skip spaces
		str = jsonSkipWhitespace(str, currentLine);

		if (!*str)
		{
			return str;
		}

		// we got a comment
		if (*str == '#')
		{
			str = jsonSkipToNextLine(str, currentLine);
		}

		str = jsonSkipWhitespace(str, currentLine);

		char expectEndChar = 0;

		// we got a string value as: "some value"
		if (*str == '"')
		{
			++str;
			// read all value string
			str = jsonReadString(str, strValue);
			value->setValue(strValue);
		}
		// we have an object value, create object
		else if (*str == '{')
		{
			++str;
			JsonObject* pObject = new JsonObject();

			value->setValue(pObject);
			str = parseObject(str, pObject);
			expectEndChar = '}';
		}
		// we have an array value
		else if (*str == '[')
		{
			++str;
			JsonArray* jsonArray = new JsonArray();

			value->setValue(jsonArray);
			str = parseArray(str, jsonArray);
			expectEndChar = ']';
		}
		else
		{
			str = jsonReadNonStringValue(str, strValue);

			if (strValue == "true" || strValue == "false")
			{
				value->setValue(strValue == "true");
			}
			else
			{
				if (jsonIsFloatStr((char*)strValue.c_str()))
				{
					value->setValue((f64)atof(strValue.c_str()));
				}
				else if (jsonIsIntStr((char*)strValue.c_str()))
				{
					u64 n = strtoull(strValue.c_str(), nullptr, 10);

					value->setValue(n);
				}
				else
				{
					value->setValue(strValue);
				}
			}
			
			return str;
		}

		str = jsonSkipWhitespace(str, currentLine);

		if (expectEndChar)
		{
			if (expectEndChar != *str)
			{
				JSON_LOG_ERROR(str, expectEndChar);
			}

			++str;
		}
	}

	return str;
}

char* JsonDocument::parseArray(char* str, JsonArray* array)
{
	while (*str && !gotErrors)
	{
		// skip spaces
		str = jsonSkipWhitespace(str, currentLine);

		if (!*str)
		{
			break;
		}

		// we got a comment
		if (*str == '#')
		{
			str = jsonSkipToNextLine(str, currentLine);
		}

		str = jsonSkipWhitespace(str, currentLine);

		char expectEndChar = 0;

		// we have an object value
		if (*str == '{')
		{
			++str;
			JsonObject* object = new JsonObject();
			JsonValue* val = new JsonValue();
			val->setValue(object);
			array->append(val);
			str = parseObject(str, object);
			expectEndChar = '}';
		}
		// we have a nested array value
		else if (*str == '[')
		{
			++str;
			JsonArray* newArray = new JsonArray();
			JsonValue* val = new JsonValue();
			val->setValue(newArray);
			array->append(val);
			str = parseArray(str, newArray);
			expectEndChar = ']';
		}
		// we have a free value
		else if (*str == '"'
			|| jsonIsIdentifierChar(*str)
			|| isdigit(*str)
			|| *str == '-')
		{
			JsonValue* val = new JsonValue();
			str = parseValue(str, val);
			array->append(val);
		}
		// skip comma
		else if (*str == ',')
		{
			str++;
		}

		str = jsonSkipWhitespace(str, currentLine);

		if (!*str)
		{
			return str;
		}

		if (expectEndChar)
		{
			if (*str == expectEndChar)
			{
				++str;
			}
			else
			{
				JSON_LOG_ERROR(str, expectEndChar);
				return str;
			}
		}

		str = jsonSkipWhitespace(str, currentLine);

		if (!*str)
		{
			return str;
		}

		// ']' ends array
		if (*str == ']')
		{
			return str;
		}
	}

	return str;
}

void writeJsonObjectToText(String& jsonText, i32 indent, JsonObject* object, JsonSaveOptions* options, bool rootSecondIndent);

void writeJsonArrayToText(String& jsonText, i32 indent, JsonArray* array, JsonSaveOptions* options, bool rootSecondIndent)
{
	if (!array)
	{
		return;
	}

	JsonSaveOptions defaultOptions;
	
	if (!options)
	{
		options = &defaultOptions;
	}

	String indentStr;
	String tmp;

	indentStr.resize(indent, '\t');
	size_t i = array->size();

	bool addScope = true;
	String localIndentTab = "\t";
	String keyValSeparator;

	switch (options->keyValueSeparator)
	{
	case JsonSaveOptions::KeyValueSeparator::Colon:
		keyValSeparator = " : ";
		break;
	case JsonSaveOptions::KeyValueSeparator::Equal:
		keyValSeparator = " = ";
		break;
	case JsonSaveOptions::KeyValueSeparator::Space:
		keyValSeparator = " ";
		break;
	default:
		break;
	}

	if (!indent && !options->rootIndentAndBraces && !rootSecondIndent)
	{
		addScope = false;
		localIndentTab = "";
		indent = -1; // trick to start from 0 indent when no root indent and scope
	}

	if (addScope)
	{
		jsonText += indentStr + "[\n";
	}

	for (size_t i = 0; i < array->size(); ++i)
	{
		JsonValue& val = *array->at(i);

		if (val.isSimpleType())
		{
			jsonText += indentStr;
			jsonText += localIndentTab;

			if (val.getType() == JsonValueType::String)
			{
				jsonText += "\"";
			}

			jsonText += val.toString();

			if (val.getType() == JsonValueType::String)
			{
				jsonText += "\"";
			}
		}
		else
		{
			if (val.isArray())
			{
				writeJsonArrayToText(jsonText, indent + 1, val.asArray(), options, true);
			}
			else
			{
				writeJsonObjectToText(jsonText, indent + 1, val.asObject(), options, true);
			}
		}

		if (i < array->size() - 1)
		{
			if (options->commaAfterValue)
			{
				jsonText += ",\n";
			}
			else
			{
				jsonText += "\n";
			}
		}
		else
		{
			jsonText += "\n";
		}
	}

	if (addScope)
	{
		jsonText += indentStr + "]";
	}
}

void writeJsonObjectToText(String& jsonText, i32 indent, JsonObject* object, JsonSaveOptions* options, bool rootSecondIndent)
{
	if (!object)
	{
		return;
	}

	JsonSaveOptions defaultOptions;

	if (!options)
	{
		options = &defaultOptions;
	}

	String indentStr;
	String tmp;

	indentStr.resize(indent, '\t');
	size_t i = object->getMembers().size();

	bool addScope = true;
	String localIndentTab = "\t";
	String keyValSeparator;

	switch (options->keyValueSeparator)
	{
	case JsonSaveOptions::KeyValueSeparator::Colon:
		keyValSeparator = " : ";
		break;
	case JsonSaveOptions::KeyValueSeparator::Equal:
		keyValSeparator = " = ";
		break;
	case JsonSaveOptions::KeyValueSeparator::Space:
		keyValSeparator = " ";
		break;
	default:
		break;
	}

	if (!indent && !options->rootIndentAndBraces && !rootSecondIndent)
	{
		addScope = false;
		localIndentTab = "";
		indent = -1; // trick to start from 0 indent when no root indent and scope
	}

	if (addScope)
	{
		jsonText += indentStr + "{\n";
	}

	for (auto iter = object->getMembers().begin(), iterEnd = object->getMembers().end(); iter != iterEnd; ++iter)
	{
		JsonValue* val = iter->value;

		bool nameContainsSpace = iter->key.findChar(' ') != String::noIndex;

		if (val->isSimpleType())
		{
			jsonText += indentStr;
			jsonText += localIndentTab;

			if (options->memberNameQuotes || nameContainsSpace)
			{
				jsonText += "\"";
			}
			
			jsonText += iter->key;

			if (options->memberNameQuotes || nameContainsSpace)
			{
				jsonText += "\"";
			}

			jsonText += keyValSeparator;

			if (val->getType() == JsonValueType::String)
			{
				jsonText += "\"";
			}

			jsonText += val->toString();

			if (val->getType() == JsonValueType::String)
			{
				jsonText += "\"";
			}
		}
		else
		{
			jsonText += indentStr;
			jsonText += localIndentTab;

			if (options->memberNameQuotes || nameContainsSpace)
			{
				jsonText += "\"";
			}

			jsonText += iter->key;

			if (options->memberNameQuotes || nameContainsSpace)
			{
				jsonText += "\"";
			}

			jsonText += keyValSeparator + "\n";

			if (val->getType() == JsonValueType::Array)
			{
				writeJsonArrayToText(jsonText, indent + 1, val->asArray(), options, true);
			}
			else
			{
				writeJsonObjectToText(jsonText, indent + 1, val->asObject(), options, true);
			}
		}

		--i;

		if (i > 0)
		{
			if (options->commaAfterValue)
			{
				jsonText += ",\n";
			}
			else
			{
				jsonText += "\n";
			}
		}
		else
		{
			jsonText += "\n";
		}
	}

	if (addScope)
	{
		jsonText += indentStr + "}";
	}
}

bool JsonDocument::save(const String& jsonFilename, JsonSaveOptions* options)
{
	FILE* file = fopen(jsonFilename.c_str(), "wt");

	if (!file)
	{
		B_LOG_ERROR("Cannot open file for writing");
		return false;
	}

	String strJson;

	saveToText(strJson, options);
	fwrite(strJson.c_str(), strJson.length(), 1, file);
	fclose(file);

	return true;
}

bool JsonDocument::saveToText(String& jsonText, JsonSaveOptions* options)
{
	writeJsonObjectToText(jsonText, 0, &root, options, false);
	
	return true;
}

JsonObject* jsonFindObjectByPathRecurse(JsonObject* object, Array<String>& pathElements, size_t elementIndex)
{
	if (!object)
	{
		return nullptr;
	}

	if (elementIndex >= pathElements.size())
	{
		return nullptr;
	}

	auto iter = object->getMembers().find(pathElements[elementIndex]);

	if (iter != object->getMembers().end())
	{
		// if last, then return, we've found it!
		if (elementIndex == pathElements.size() - 1)
		{
			return iter->value->asObject();
		}
		else
		{
			return jsonFindObjectByPathRecurse(iter->value->asObject(), pathElements, elementIndex + 1);
		}
	}
	
	return nullptr;
}

JsonObject* JsonDocument::getObject(const String& path)
{
	Array<String> pathElements;
	
	explodeString(path, pathElements, "/");
	
	return jsonFindObjectByPathRecurse(&root, pathElements, 0);
}

JsonValue* jsonFindValueByPathRecurse(JsonObject* node, Array<String>& pathElements, size_t elementIndex);

JsonArray* JsonDocument::getArray(const String& path)
{
	Array<String> pathElements;

	explodeString(path, pathElements, "/");
	JsonValue* val = jsonFindValueByPathRecurse(&root, pathElements, 0);

	if (val)
	{
		if (val->isArray())
		{
			return val->asArray();
		}
	}

	return nullptr;
}

JsonValue* jsonFindValueByPathRecurse(JsonObject* node, Array<String>& pathElements, size_t elementIndex)
{
	if (!node)
	{
		return nullptr;
	}
	
	if (elementIndex >= pathElements.size())
	{
		return nullptr;
	}

	auto iter = node->getMembers().begin();
	
	while (iter != node->getMembers().end())
	{
		if (iter->key == pathElements[elementIndex])
		{
			// if last, then return, we've found it!
			if (elementIndex == pathElements.size() - 1)
			{
				return iter->value;
			}
			else
			{
				return jsonFindValueByPathRecurse(iter->value->asObject(), pathElements, elementIndex + 1);
			}
		}

		++iter;
	}

	return nullptr;
}

JsonValue* JsonDocument::getValue(const String& path)
{
	Array<String> pathElements;

	explodeString(path, pathElements, "/");

	return jsonFindValueByPathRecurse(&root, pathElements, 0);
}

String JsonDocument::getString(const String& path, const String& defaultValue)
{
	JsonValue* val = getValue(path);

	if (!val)
	{
		return defaultValue;
	}

	return val->asString();
}

i32 JsonDocument::getI32(const String& path, i32 defaultValue)
{
	JsonValue* val = getValue(path);

	if (!val)
	{
		return defaultValue;
	}

	return val->asI32();
}

u32 JsonDocument::getU32(const String& path, u32 defaultValue)
{
	JsonValue* val = getValue(path);

	if (!val)
	{
		return defaultValue;
	}

	return val->asU32();
}

i64 JsonDocument::getI64(const String& path, i64 defaultValue)
{
	JsonValue* val = getValue(path);

	if (!val)
	{
		return defaultValue;
	}

	return val->asI64();
}

u64 JsonDocument::getU64(const String& path, u64 defaultValue)
{
	JsonValue* val = getValue(path);

	if (!val)
	{
		return defaultValue;
	}

	return val->asU64();
}

bool JsonDocument::getBool(const String& path, bool defaultValue)
{
	JsonValue* val = getValue(path);

	if (!val)
	{
		return defaultValue;
	}

	return val->asBool();
}

f32 JsonDocument::getF32(const String& path, f32 defaultValue)
{
	JsonValue* val = getValue(path);

	if (!val)
	{
		return defaultValue;
	}

	return val->asF32();
}

f64 JsonDocument::getF64(const String& path, f64 defaultValue)
{
	JsonValue* val = getValue(path);

	if (!val)
	{
		return defaultValue;
	}

	return val->asF64();
}

JsonObject& JsonDocument::getRoot()
{
	return root;
}

const String& JsonDocument::getFilename() const
{
	return filename;
}

bool JsonDocument::hasErrors() const
{
	return gotErrors;
}

u32 JsonDocument::getErrorLine() const
{
	return currentLine;
}

const String& JsonDocument::getErrorMessage() const
{
	return errorText;
}

void JsonDocument::setError(char* str, u32 cppLine, const String& expecting)
{
	errorText = "";
	errorText << "JSON: Expecting " << expecting << ", but got char '" << (char)*str << "' at line #" << currentLine;
	gotErrors = true;
	B_LOG_ERROR(errorText);
}

void JsonDocument::setError(char* str, u32 cppLine, char expecting)
{
	errorText = "";
	errorText << "JSON: Expecting '" << expecting << "', but got char '" << (char)*str << "' at line #" << currentLine;
	gotErrors = true;
	B_LOG_ERROR(errorText);
}

void JsonDocument::beginDocument()
{
	free();
	currentWriteObject = &root;
	currentWriteArray = nullptr;
	writeTypeStack.clear();
	writeStack.clear();
	currentWriteValueType = JsonValueType::Object;
}

void JsonDocument::endDocument()
{}

void JsonDocument::beginObject(const String& name)
{
	JsonObject* obj = new JsonObject();

	if (currentWriteValueType == JsonValueType::Object)
	{
		writeStack.append(currentWriteObject);
	}
	else if (currentWriteValueType == JsonValueType::Array)
	{
		writeStack.append(currentWriteArray);
	}

	writeTypeStack.append(currentWriteValueType);
	writeKeyStack.append(name);
	currentWriteValueType = JsonValueType::Object;
	currentWriteObject = obj;
}

void JsonDocument::endObject()
{
	JsonValue* val = new JsonValue();
	void* parent = nullptr;
	String key;

	val->setValue(currentWriteObject);
	writeStack.popLast(parent);
	writeTypeStack.popLast(currentWriteValueType);
	writeKeyStack.popLast(key);
	
	if (parent)
	{
		if (currentWriteValueType == JsonValueType::Array)
		{
			JsonArray* array = (JsonArray*)parent;
			array->append(val);
			currentWriteArray = array;
		}
		else if (currentWriteValueType == JsonValueType::Object)
		{
			JsonObject* object = (JsonObject*)parent;
			object->setValue(key, val);
			currentWriteObject = object;
		}
	}
}

void JsonDocument::beginArray(const String& name)
{
	JsonArray* array = new JsonArray();

	if (currentWriteValueType == JsonValueType::Array)
	{
		writeStack.append(currentWriteArray);
	}
	else if (currentWriteValueType == JsonValueType::Object)
	{
		writeStack.append(currentWriteObject);
	}

	writeTypeStack.append(currentWriteValueType);
	writeKeyStack.append(name);
	currentWriteValueType = JsonValueType::Array;
	currentWriteArray = array;
}

void JsonDocument::endArray()
{
	JsonValue* val = new JsonValue();
	void* parent = nullptr;
	String key;

	val->setValue(currentWriteArray);
	writeStack.popLast(parent);
	writeTypeStack.popLast(currentWriteValueType);
	writeKeyStack.popLast(key);

	if (parent)
	{
		if (currentWriteValueType == JsonValueType::Array)
		{
			JsonArray* array = (JsonArray*)parent;
			array->append(val);
			currentWriteArray = array;
		}
		else if (currentWriteValueType == JsonValueType::Object)
		{
			JsonObject* object = (JsonObject*)parent;
			object->setValue(key, val);
			currentWriteObject = object;
		}
	}
}

void JsonDocument::addValue(const String& name, i32 value)
{
	if (currentWriteObject)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteObject->setValue(name, val);
	}
}

void JsonDocument::addValue(const String& name, u32 value)
{
	if (currentWriteObject)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteObject->setValue(name, val);
	}
}

void JsonDocument::addValue(const String& name, i64 value)
{
	if (currentWriteObject)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteObject->setValue(name, val);
	}
}

void JsonDocument::addValue(const String& name, u64 value)
{
	if (currentWriteObject)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteObject->setValue(name, val);
	}
}

void JsonDocument::addValue(const String& name, bool value)
{
	if (currentWriteObject)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteObject->setValue(name, val);
	}
}

void JsonDocument::addValue(const String& name, f32 value)
{
	if (currentWriteObject)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteObject->setValue(name, val);
	}
}

void JsonDocument::addValue(const String& name, f64 value)
{
	if (currentWriteObject)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteObject->setValue(name, val);
	}
}

void JsonDocument::addValue(const String& name, const String& value)
{
	if (currentWriteObject)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteObject->setValue(name, val);
	}
}

void JsonDocument::addValue(const String& name, const char* value)
{
	addValue(name, String(value));
}

void JsonDocument::addValue(const String& value)
{
	if (currentWriteArray)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteArray->append(val);
	}
}

void JsonDocument::addValue(i32 value)
{
	if (currentWriteArray)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteArray->append(val);
	}
}

void JsonDocument::addValue(u32 value)
{
	if (currentWriteArray)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteArray->append(val);
	}
}

void JsonDocument::addValue(i64 value)
{
	if (currentWriteArray)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteArray->append(val);
	}
}

void JsonDocument::addValue(u64 value)
{
	if (currentWriteArray)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteArray->append(val);
	}
}

void JsonDocument::addValue(bool value)
{
	if (currentWriteArray)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteArray->append(val);
	}
}

void JsonDocument::addValue(f32 value)
{
	if (currentWriteArray)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteArray->append(val);
	}
}

void JsonDocument::addValue(f64 value)
{
	if (currentWriteArray)
	{
		JsonValue* val = new JsonValue();
		val->setValue(value);
		currentWriteArray->append(val);
	}
}

#ifdef _DEBUG

void debugJsonValue(JsonValue* val, i32 indent)
{
	String spacing, str;
	
	spacing.resize(indent * 4, ' ');

	if (val->isSimpleType())
	{
		B_LOG_DEBUG(spacing << val->toString());
	}
	else
	{
		if (val->isArray())
		{
			auto array = val->asArray();
			
			for (auto val2 : *array)
			{
				debugJsonValue(val2, indent + 1);
			}
		}
		else if (val->isObject())
		{
			auto obj = val->asObject();

			for (auto mem : obj->getMembers())
			{
				B_LOG_DEBUG(spacing << mem.key);
				debugJsonValue(mem.value, indent + 1);
			}
		}
	}
}

void JsonDocument::debug()
{
	for (auto mem : root.getMembers())
	{
		B_LOG_DEBUG(mem.key);
		debugJsonValue(mem.value, 1);
	}
}

#endif

}