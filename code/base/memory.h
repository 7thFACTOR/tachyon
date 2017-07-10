#pragma once
#include "base/defines.h"
#include "base/types.h"

namespace base
{
#ifdef _WINDOWS
#define B_ALIGNOF(type) __alignof(type)
#define B_ALIGN(size) __declspec(align(size))
#else 
#define B_ALIGNOF(type) alignof(type)
#define B_ALIGN(size) __attribute__((aligned(size)))
#endif

class B_API MemoryAllocator
{
public:
	virtual void* allocate(size_t size, size_t align = 0) = 0;
	virtual void deallocate(void* addr) = 0;
	virtual size_t getAllocatedSize(void* addr) = 0;
	virtual size_t getAllocationCount() = 0;
	virtual size_t getTotalAllocatedSize() = 0;

	template <typename Type>
	Type* newObject()
	{ return new (allocate(sizeof(Type), B_ALIGNOF(Type))) Type(); }

	template <typename Type, typename _Ty1>
	Type* newObject(const _Ty1& arg1)
	{ return new (allocate(sizeof(Type), B_ALIGNOF(Type))) Type(arg1); }

	template <typename Type, typename _Ty1, typename _Ty2>
	Type* newObject(const _Ty1& arg1, const _Ty2& arg2)
	{ return new (allocate(sizeof(Type), B_ALIGNOF(Type))) Type(arg1, arg2); }

	template <typename Type, typename _Ty1, typename _Ty2, typename _Ty3>
	Type* newObject(const _Ty1& arg1, const _Ty2& arg2, const _Ty3& arg3)
	{ return new (allocate(sizeof(Type), B_ALIGNOF(Type))) Type(arg1, arg2, arg3); }

	template <typename Type, typename _Ty1, typename _Ty2, typename _Ty3, typename _Ty4>
	Type* newObject(const _Ty1& arg1, const _Ty2& arg2, const _Ty3& arg3, const _Ty4& arg4)
	{ return new (allocate(sizeof(Type), B_ALIGNOF(Type))) Type(arg1, arg2, arg3, arg4); }

	template <typename Type, typename _Ty1, typename _Ty2, typename _Ty3, typename _Ty4, typename _Ty5>
	Type* newObject(const _Ty1& arg1, const _Ty2& arg2, const _Ty3& arg3, const _Ty4& arg4, const _Ty5& arg5)
	{ return new (allocate(sizeof(Type), B_ALIGNOF(Type))) Type(arg1, arg2, arg3, arg4, arg5); }

	template <typename Type>
	Type* newObjectArray(size_t count)
	{
		Type* ptr = allocate(count * sizeof(Type), B_ALIGNOF(Type));
		for (size_t i = 0; i < count; ++i) new (ptr + i) Type();
		return ptr; 
	}

	template <typename Type, typename _Ty1>
	Type* newObjectArray(size_t count, const _Ty1& arg1)
	{
		Type* ptr = allocate(count * sizeof(Type), B_ALIGNOF(Type));
		for (size_t i = 0; i < count; ++i) new (ptr + i) Type(arg1);
		return ptr;
	}

	template <typename Type, typename _Ty1, typename _Ty2>
	Type* newObjectArray(size_t count, const _Ty1& arg1, const _Ty2& arg2)
	{
		Type* ptr = allocate(count * sizeof(Type), B_ALIGNOF(Type));
		for (size_t i = 0; i < count; ++i) new (ptr + i) Type(arg1, arg2); 
		return ptr;
	}

	template <typename Type, typename _Ty1, typename _Ty2, typename _Ty3>
	Type* newObjectArray(size_t count, const _Ty1& arg1, const _Ty2& arg2, const _Ty3& arg3)
	{
		Type* ptr = allocate(count * sizeof(Type), B_ALIGNOF(Type));
		for (size_t i = 0; i < count; ++i) new (ptr + i) Type(arg1, arg2, arg3);
		return ptr;
	}

	template <typename Type, typename _Ty1, typename _Ty2, typename _Ty3, typename _Ty4>
	Type* newObjectArray(size_t count, const _Ty1& arg1, const _Ty2& arg2, const _Ty3& arg3, const _Ty4& arg4)
	{
		Type* ptr = allocate(count * sizeof(Type), B_ALIGNOF(Type));
		for (size_t i = 0; i < count; ++i) new (ptr + i) Type(arg1, arg2, arg3, arg4);
		return ptr;
	}

	template <typename Type, typename _Ty1, typename _Ty2, typename _Ty3, typename _Ty4, typename _Ty5>
	Type* newObjectArray(size_t count, const _Ty1& arg1, const _Ty2& arg2, const _Ty3& arg3, const _Ty4& arg4, const _Ty5& arg5)
	{
		Type* ptr = allocate(count * sizeof(Type), B_ALIGNOF(Type));
		for (size_t i = 0; i < count; ++i) new (ptr + i) Type(arg1, arg2, arg3, arg4, arg5);
		return ptr;
	}

	template <typename Type>
	void deleteObject(Type* addr) 
	{
		if (addr) 
		{
			addr->~Type();
			deallocate(addr);
		}
	}

	template <typename Type>
	void deleteObjectArray(size_t count, Type* addr) 
	{
		if (addr)
		{
			for (size_t i = 0; i < count; ++i)
			{
				(addr + i).~Type();
			}

			deallocate(addr);
		}
	}
};

B_EXPORT MemoryAllocator* getMainAllocator();
B_EXPORT void deleteMainAllocator();
}

#define B_NEW(type,...) getMainAllocator()->newObject<type>(__VA_ARGS__)
#define B_NEW_ARRAY(type, count,...) getMainAllocator()->newObjectArray<type>(count, __VA_ARGS__)
#define B_DELETE(ptr) getMainAllocator()->deleteObject(ptr)
#define B_DELETE_ARRAY(count, ptr) getMainAllocator()->deleteObjectArray(count, ptr)

#ifdef B_USE_CUSTOM_ALLOCATORS
void* operator new(size_t amount);
void* operator new[](size_t amount);
void operator delete(void* ptr);
void operator delete[](void* ptr);
#endif