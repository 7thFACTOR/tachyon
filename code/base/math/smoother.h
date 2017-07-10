#pragma once
#include "base/defines.h"
#include "base/math/util.h"

namespace base
{
const size_t defaultSmootherSampleCount = 5;

//! This class is used to get a smoothed value out of an array of values
template<typename Type>
class Smoother
{
public:
	Smoother()
	{
		values.resize(defaultSmootherSampleCount);
		values.fill(0, defaultSmootherSampleCount, Type());
		rotator = 0;
	}

	~Smoother()
	{}

	void addValue(Type value)
	{
		values[rotator++] = value;

		if (rotator >= values.size())
		{
			rotator = 0;
		}
	}

	void setSampleCount(size_t count)
	{
		values.resize(count);
		rotator = clampValue(rotator, (size_t)0, (size_t)(values.size() - 1));
	}

	Type compute()
	{
		if (values.isEmpty())
		{
			return Type();
		}

		Type sum = 0;

		for (size_t i = 0, iCount = values.size(); i < iCount; ++i)
		{
			sum += values[i];
		}

		return (Type) sum / values.size();
	}

	void reset()
	{
		rotator = 0;
		values.fill(0, values.size(), Type());
	}

protected:
	Array<Type> values;
	size_t rotator;
};

typedef Smoother<f32> SmootherF32;
typedef Smoother<i32> SmootherI32;
typedef Smoother<Vec3> SmootherVec3;

}