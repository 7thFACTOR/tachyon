// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/attributes.h"
#include "base/stream.h"

namespace base
{
Attributes::Attributes()
{}

Attributes::~Attributes()
{
}

void Attributes::setAttribute(const String& name, const String& value)
{
	auto iter = attributes.find(name);

	if (iter == attributes.end())
	{
		attributes.add(name, value);
		return;
	}

	attributes[name] = value;
}

void Attributes::removeAttribute(const String& name)
{
	auto iter = attributes.find(name);

	if (iter != attributes.end())
	{
		attributes.erase(iter);
	}
}

const String& Attributes::getAttributeValue(const String& name)
{
	auto iter = attributes.find(name);

	if (iter != attributes.end())
	{
		return iter->value.c_str();
	}

	static String dummy;

	return dummy;
}

void Attributes::deleteAttributes()
{
	attributes.clear();
}

size_t Attributes::getAttributeCount() const
{
	return attributes.size();
}

const String& Attributes::getAttributeNameAt(size_t index) const
{
	return attributes.keyAt(index);
}

const String& Attributes::getAttributeValueAt(size_t index) const
{
	return attributes.valueAt(index);
}

void Attributes::serialize(Stream& stream)
{
	stream.writeNumber(attributes.size());
	auto iter = attributes.begin();

	while (iter != attributes.end())
	{
		stream.writeString(iter->key);
		stream.writeString(iter->value);
		iter++;
	}
}

void Attributes::deserialize(Stream& stream)
{
	size_t attrCount = 0;
	String name, value;

	attributes.clear();
	stream.readNumber(attrCount);

	for (size_t i = 0; i < attrCount; i++)
	{
		stream.readString(name);
		stream.readString(value);
		setAttribute(name, value);
	}
}

Attributes& Attributes::operator= (const Attributes& other)
{
	attributes = other.attributes;

	return *this;
}

}
