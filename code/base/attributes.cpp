#include "base/attributes.h"
#include "base/stream.h"

namespace base
{
Attributes::Attributes()
{}

Attributes::~Attributes()
{
}

void Attributes::setAttribute(const char* name, const char* value)
{
	auto iter = attributes.find(name);

	if (iter == attributes.end())
	{
		attributes.add(name, value);
		return;
	}

	attributes[name] = value;
}

void Attributes::removeAttribute(const char* name)
{
	auto iter = attributes.find(name);

	if (iter != attributes.end())
	{
		attributes.erase(iter);
	}
}

const char* Attributes::getAttributeValue(const char* name)
{
	auto iter = attributes.find(name);

	if (iter != attributes.end())
	{
		return iter->value.c_str();
	}

	return "";
}

void Attributes::deleteAttributes()
{
	attributes.clear();
}

size_t Attributes::getAttributeCount() const
{
	return attributes.size();
}

const char* Attributes::getAttributeNameAt(size_t index) const
{
	return attributes.keyAt(index).c_str();
}

const char* Attributes::getAttributeValueAt(size_t index) const
{
	return attributes.valueAt(index).c_str();
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
		setAttribute(name.c_str(), value.c_str());
	}
}

Attributes& Attributes::operator= (const Attributes& other)
{
	attributes = other.attributes;

	return *this;
}

}
