#ifdef _LINUX
#include "base/atomic.h"
#include <pthread.h>
#include <stdlib.h>

namespace base
{
AtomicInteger::AtomicInteger()
{
	__atomic_exchange_n(&value, 0, __ATOMIC_RELAXED);
}

AtomicInteger::AtomicInteger(int val)
{
	__atomic_exchange_n(&value, val, __ATOMIC_RELAXED);
}

void AtomicInteger::set(int val)
{
	__atomic_exchange_n(&value, val, __ATOMIC_RELAXED);
}

int AtomicInteger::read()
{
	return __atomic_fetch_add(&value, 0, __ATOMIC_RELAXED);
}

void AtomicInteger::add(int val)
{
	__atomic_fetch_add(&value, val, __ATOMIC_RELAXED);
}

void AtomicInteger::subtract(int val)
{
	__atomic_fetch_sub(&value, val, __ATOMIC_RELAXED);
}

void AtomicInteger::increment()
{
	__atomic_add_fetch(&value, 1, __ATOMIC_RELAXED);
}

void AtomicInteger::decrement()
{
	__atomic_sub_fetch(&value, 1, __ATOMIC_RELAXED);
}

bool AtomicInteger::subtractAndTest(int val)
{
	return __atomic_fetch_sub(&value, val, __ATOMIC_RELAXED) - val <= 0;
}

bool AtomicInteger::addNegative(int val)
{
	return __atomic_fetch_add(&value, val, __ATOMIC_RELAXED) + val < 0;
}

bool AtomicInteger::decrementAndTest()
{
	return __atomic_sub_fetch(&value, 1, __ATOMIC_RELAXED) == 0;
}

bool AtomicInteger::incrementAndTest()
{
	return __atomic_add_fetch(&value, 1, __ATOMIC_RELAXED) == 0;
}

}

#endif