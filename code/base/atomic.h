// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"

namespace base
{
class B_API AtomicInteger
{
public:
	AtomicInteger();
	AtomicInteger(int val);
	void set(int val);
	int read();
	void add(int val);
	void subtract(int val);
	void increment();
	void decrement();
	bool subtractAndTest(int val);
	bool addNegative(int val);
	bool decrementAndTest();
	bool incrementAndTest();
	operator bool() { return read() != 0; }
	operator int() { return read(); }

	AtomicInteger& operator++() { increment(); return *this; }
	AtomicInteger& operator--() { decrement(); return *this; }
	AtomicInteger operator++(int) { AtomicInteger tmp = *this; increment(); return tmp; }
	AtomicInteger operator--(int) { AtomicInteger tmp = *this; decrement(); return tmp; }
	AtomicInteger& operator+=(int val) { add(val); return *this; }
	AtomicInteger& operator-=(int val) { subtract(val); return *this; }

protected:
#ifdef _WINDOWS
	__declspec(align(32)) volatile long value;
#else
	volatile long value;
#endif
};

}