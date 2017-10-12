// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once

namespace base
{
template<typename Type>
class Range
{
public:
	Type offset = 0;
	Type count = 0;

	inline Type getEnd() const { return offset + count; }

	Range() {}
	Range(Type rangeOffset, Type rangeCount)
		: offset(rangeOffset)
		, count(rangeCount)
	{}
};

typedef Range<i8> RangeI8;
typedef Range<u8> RangeU8;
typedef Range<i16> RangeI16;
typedef Range<u16> RangeU16;
typedef Range<i32> RangeI32;
typedef Range<u32> RangeU32;
typedef Range<i64> RangeI64;
typedef Range<u64> RangeU64;
typedef Range<size_t> RangeSizeT;
typedef Range<f32> RangeF32;
typedef Range<f64> RangeF64;

}