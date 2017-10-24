// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/string.h"
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/key_value_pair.h"

namespace base
{
class JsonDocument;

enum class JsonValueType
{
	Null,
	Integer,
	Float,
	String,
	Bool,
	Object,
	Array
};

class JsonValue;
class JsonObject;

typedef Array<JsonValue*> JsonArray;

class B_API JsonValue
{
public:
	JsonValue()
		: type(JsonValueType::Null)
		, value(nullptr)
	{}

	JsonValue(const JsonValue& other)
		: type(JsonValueType::Null)
		, value(nullptr)
	{
		*this = other;
	}

	~JsonValue()
	{
		free();
	}

	const JsonValue& operator = (const JsonValue& other);
	i32 asI32() const;
	u32 asU32() const;
	i64 asI64() const;
	u64 asU64() const;
	f32 asF32() const;
	f64 asF64() const;
	bool asBool() const;
	const String& asString() const;
	JsonObject* asObject() const;
	JsonArray* asArray() const;
	String toString() const;
	void setValue(i32 value);
	void setValue(u32 value);
	void setValue(i64 value);
	void setValue(u64 value);
	void setValue(f32 value);
	void setValue(f64 value);
	void setValue(bool value);
	void setValue(const String& value);
	void setValue(JsonArray* value);
	void setValue(JsonObject* value);
	void assignValue(JsonValueType type, void* value);
	void free();
	inline void setType(JsonValueType newType) { type = newType; }
	inline bool isSimpleType() const { return type != JsonValueType::Object && type != JsonValueType::Array; }
	inline bool isArray() const { return type == JsonValueType::Array; }
	inline bool isObject() const { return type == JsonValueType::Object; }
	inline JsonValueType getType() const { return type; }

protected:
	JsonValueType type;
	void* value;
};

class B_API JsonObject
{
public:
	friend class JsonDocument;
	typedef Array<KeyValuePair<String, JsonValue*> > NameValuePairs;

	JsonObject() {}
	JsonObject(const JsonObject& other) { members = other.members; }
	~JsonObject() { free(); }
	const JsonObject& operator = (const JsonObject& other) { members = other.members; return *this; }
	bool isEmpty() const;
	JsonValue* getValueOf(const String& key) const;
	String getString(const String& name, const String& defaultValue = "") const;
	i32 getI32(const String& name, i32 defaultValue = 0) const;
	u32 getU32(const String& name, u32 defaultValue = 0) const;
	i64 getI64(const String& name, i64 defaultValue = 0) const;
	u64 getU64(const String& name, u64 defaultValue = 0) const;
	f32 getF32(const String& name, f32 defaultValue = 0.0f) const;
	f64 getF64(const String& name, f64 defaultValue = 0.0f) const;
	bool getBool(const String& name, bool defaultValue = false) const;
	JsonObject* getObject(const String& name) const;
	JsonArray* getArray(const String& name) const;
	bool hasKey(const String& key) const;
	const NameValuePairs& getMembers() const;
	void free();
	void setValue(const String& name, JsonValue* value);

protected:
	JsonValue* getValuePtr(const String& name);

	NameValuePairs members;
};

struct JsonSaveOptions
{
	enum class KeyValueSeparator
	{
		Colon,
		Equal,
		Space
	};

	bool memberNameQuotes = true;
	bool simpleValueQuotes = true;
	bool rootIndentAndBraces = true;
	KeyValueSeparator keyValueSeparator = KeyValueSeparator::Colon;
	bool commaAfterValue = true;

	static JsonSaveOptions sjsonOptions()
	{
		JsonSaveOptions o;
		
		o.commaAfterValue = false;
		o.keyValueSeparator = KeyValueSeparator::Equal;
		o.rootIndentAndBraces = false;
		o.memberNameQuotes = false;
		o.simpleValueQuotes = false;

		return o;
	}
};

class B_API JsonDocument
{
public:
	friend class JsonObject;

	JsonDocument();
	JsonDocument(const String& filename, bool parse = true);
	virtual ~JsonDocument();

	//! load game info from file, but its not parsing it
	bool loadTextOnly(const String& filename);
	bool loadAndParse(const String& filename);
	bool save(const String& filename, JsonSaveOptions* options = nullptr);
	bool saveToText(String& jsonText, JsonSaveOptions* options = nullptr);
	bool parse();
	void free();
	void setJsonText(const String& text);
	const String& getJsonText() const;
	JsonObject* getObject(const String& path);
	JsonArray* getArray(const String& path);
	JsonValue* getValue(const String& path);
	String getString(const String& path, const String& defaultValue = "");
	i32 getI32(const String& path, i32 defaultValue = 0);
	u32 getU32(const String& path, u32 defaultValue = 0);
	i64 getI64(const String& path, i64 defaultValue = 0);
	u64 getU64(const String& path, u64 defaultValue = 0);
	f32 getF32(const String& path, f32 defaultValue = 0.0f);
	f64 getF64(const String& path, f64 defaultValue = 0.0f);
	bool getBool(const String& path, bool defaultValue = false);

	JsonObject& getRoot();
	const String& getFilename() const;
	bool hasErrors() const;
	u32 getErrorLine() const;
	const String& getErrorMessage() const;

	void beginDocument();
	void endDocument();

	void beginObject(const String& name = "");
	void endObject();

	void beginArray(const String& name = "");
	void endArray();

	// used for adding object members
	void addValue(const String& name, i32 value);
	void addValue(const String& name, u32 value);
	void addValue(const String& name, i64 value);
	void addValue(const String& name, u64 value);
	void addValue(const String& name, f32 value);
	void addValue(const String& name, f64 value);
	void addValue(const String& name, bool value);
	void addValue(const String& name, const String& value);
	void addValue(const String& name, const char* value);

	// used for adding values to an array
	void addValue(const String& value);
	void addValue(i32 value);
	void addValue(u32 value);
	void addValue(i64 value);
	void addValue(u64 value);
	void addValue(f32 value);
	void addValue(f64 value);
	void addValue(bool value);

#ifdef _DEBUG
	void debug();
#endif

protected:
	Utf8Byte* parseObject(Utf8Byte* str, Utf8Byte* end, JsonObject* parent);
	Utf8Byte* parseValue(Utf8Byte* str, Utf8Byte* end, JsonValue* value);
	Utf8Byte* parseArray(Utf8Byte* str, Utf8Byte* end, JsonArray* arr);
	void setError(Utf8Byte* str, Utf8Byte* end, u32 cppLine, const String& expectingChar);
	void setError(Utf8Byte* str, Utf8Byte* end, u32 cppLine, const Utf32Codepoint expectingChar);
	
	String json;
	JsonObject root;
	String filename;
	u32 currentLine = 0;
	bool gotErrors = false;
	JsonObject* currentWriteObject = nullptr;
	JsonArray* currentWriteArray = nullptr;
	JsonValue currentWriteValue;
	Array<void*> writeStack;
	Array<JsonValueType> writeTypeStack;
	Array<String> writeKeyStack;
	JsonValueType currentWriteValueType = JsonValueType::Null;
	String errorText;
};

}