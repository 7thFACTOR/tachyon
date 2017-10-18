// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/serialize.h"
#include "base/dictionary.h"
#include "base/string.h"

namespace base
{
class Stream;

//! Attributes are a set of custom defined text key-value pairs
class B_API Attributes : public Serializable
{
public:
	Attributes();
	virtual ~Attributes();

	void setAttribute(const String& name, const String& value);
	void removeAttribute(const String& name);
	const String& getAttributeValue(const String& name);
	void deleteAttributes();
	size_t getAttributeCount() const;
	const String& getAttributeNameAt(size_t index) const;
	const String& getAttributeValueAt(size_t index) const;
	void serialize(Stream& stream) override;
	void deserialize(Stream& stream) override;

	Attributes& operator = (const Attributes& other);

protected:
	Dictionary<String, String> attributes;
};

}
