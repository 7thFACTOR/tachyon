#include "base/parameter_list.h"
#include "base/platform.h"
#include "base/assert.h"

namespace base
{
static Variant dummyVariant;

ParameterList::ParameterList()
{}

ParameterList::~ParameterList()
{}

void ParameterList::add(const Variant& value)
{
	parameters.append(value);
}

void ParameterList::setParameterValue(u32 index, const Variant& arg)
{
	B_ASSERT(index < parameters.size());

	if (index >= parameters.size())
		return;

	parameters[index] = arg;
}

const Variant& ParameterList::getParameterValue(u32 index) const
{
	B_ASSERT(index < parameters.size());

	if (index >= parameters.size())
	{
		return dummyVariant;
	}

	return parameters[index];
}

ParameterList& ParameterList::operator = (const ParameterList& other)
{
	parameters = other.getParameters();
	return *this;
}

ParameterList& ParameterList::operator << (const Variant& newVar)
{
	add(newVar);
	return *this;
}

}