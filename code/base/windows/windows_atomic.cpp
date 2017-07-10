#ifdef _WINDOWS
#include "base/atomic.h"
#include <windows.h>

namespace base
{
AtomicInteger::AtomicInteger()
{
	InterlockedExchange(&value, 0);
}

AtomicInteger::AtomicInteger(int val)
{
	InterlockedExchange(&value, val);
}

void AtomicInteger::set(int val)
{
	InterlockedExchange(&value, val);
}

int AtomicInteger::read()
{
	return InterlockedExchangeAdd(&value, 0);
}

void AtomicInteger::add(int val)
{
	InterlockedExchangeAdd(&value, val);
}

void AtomicInteger::subtract(int val)
{
	InterlockedExchangeSubtract((unsigned long*)&value, val);
}

void AtomicInteger::increment()
{
	InterlockedIncrement(&value);
}

void AtomicInteger::decrement()
{
	InterlockedDecrement(&value);
}

bool AtomicInteger::subtractAndTest(int val)
{
	return InterlockedExchangeSubtract((unsigned long*)&value, val) - val <= 0;
}

bool AtomicInteger::addNegative(int val)
{
	return InterlockedExchangeAdd(&value, val) + val < 0;
}

bool AtomicInteger::decrementAndTest()
{
	return InterlockedDecrement(&value) == 0;
}

bool AtomicInteger::incrementAndTest()
{
	return InterlockedIncrement(&value) == 0;
}

}
#endif