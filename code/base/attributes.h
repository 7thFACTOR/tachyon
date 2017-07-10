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

	void setAttribute(const char* name, const char* value);
	void removeAttribute(const char* name);
	const char* getAttributeValue(const char* name);
	void deleteAttributes();
	size_t getAttributeCount() const;
	const char* getAttributeNameAt(size_t index) const;
	const char* getAttributeValueAt(size_t index) const;
	void serialize(Stream& stream) override;
	void deserialize(Stream& stream) override;

	Attributes& operator = (const Attributes& other);

protected:
	Dictionary<String, String> attributes;
};

}
