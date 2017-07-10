#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/array.h"
#include "base/variant.h"

namespace base
{
class B_API ParameterList
{
public:
	ParameterList();
	ParameterList(const ParameterList& other)
	{
		*this = other;
	}
	~ParameterList();

	void add(const Variant& arg);
	inline const Array<Variant>& getParameters() const { return parameters; }
	void setParameterValue(u32 index, const Variant& value);
	const Variant& getParameterValue(u32 index) const;

	ParameterList& operator = (const ParameterList& other);
	ParameterList& operator << (const Variant& newVar);

protected:
	Array<Variant> parameters;
};

}