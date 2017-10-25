// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include <cstddef>

using namespace std;

namespace base
{
#define B_DEFAULT_ALLOCATOR_NAME "DefaultAllocator"

class Allocator
{
public:
	Allocator(const char* name = B_DEFAULT_ALLOCATOR_NAME);
	Allocator(const Allocator& other);
	Allocator(const Allocator& other, const char* name);

	Allocator& operator = (const Allocator& other);

	void* allocate(size_t n, int flags = 0);
	void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0);
	void deallocate(void* p, size_t n);

	const char* getName() const;
	void setName(const char* name);

protected:
#ifdef B_ENABLE_ALLOCATOR_NAME
	static const int maxNameSize = 64;
	char name[maxNameSize] = {0};
#endif
};

// Implementation

inline Allocator::Allocator(const char* name)
{
#if B_ENABLE_ALLOCATOR_NAME
	strcpy_s(name, maxNameSize, other.name);
#endif
}

inline Allocator::Allocator(const Allocator& other)
{
#if B_ENABLE_ALLOCATOR_NAME
	strcpy_s(name, maxNameSize, other.name);
#endif
}

inline Allocator::Allocator(const Allocator&, const char* name)
{
#if B_ENABLE_ALLOCATOR_NAME
	strcpy_s(name, maxNameSize, other.name);
#endif
}

inline Allocator& Allocator::operator = (const Allocator& other)
{
#if B_ENABLE_ALLOCATOR_NAME
	strcpy_s(name, maxNameSize, other.name);
#endif
	return *this;
}

inline const char* Allocator::getName() const
{
#if B_ENABLE_ALLOCATOR_NAME
	return name;
#endif
	return B_DEFAULT_ALLOCATOR_NAME;
}

inline void Allocator::setName(const char* newName)
{
#if B_ENABLE_ALLOCATOR_NAME
	strcpy_s(name, maxNameSize, other.name);
#endif
}

inline void* Allocator::allocate(size_t n, int flags)
{
	//TODO: use name to log
	// if DLL then use an internal function, because new might be overridden
	return new char[n];
}

inline void* Allocator::allocate(size_t n, size_t alignment, size_t offset, int flags)
{
	//TODO
	B_ASSERT_NOT_IMPLEMENTED
	return nullptr;
	//#define B_PLATFORM_PTR_SIZE sizeof(void*)
	//size_t adjustedAlignment = (alignment > B_PLATFORM_PTR_SIZE) ? alignment : B_PLATFORM_PTR_SIZE;

	//void* p = new char[n + adjustedAlignment + B_PLATFORM_PTR_SIZE];
	//void* plusPointerSize = (void*)((uintptr_t)p + B_PLATFORM_PTR_SIZE);
	//void* aligned = (void*)(((uintptr_t)plusPointerSize + adjustedAlignment - 1) & ~(adjustedAlignment - 1));

	//void** storedPtr = (void**)aligned - 1;
	//*(storedPtr) = p;

	//B_ASSERT(((size_t)aligned & ~(alignment - 1)) == (size_t)aligned);

	//return aligned;
}

inline void Allocator::deallocate(void* p, size_t)
{
	//TODO: deal with alignment and stored ptr when in DLL (internal alloc/dealloc)
	delete [](char*)p;
}

inline bool operator == (const Allocator&, const Allocator&)
{
	return true;
}


inline bool operator != (const Allocator&, const Allocator&)
{
	return false;
}

}
