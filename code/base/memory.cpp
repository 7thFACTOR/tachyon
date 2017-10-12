// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "base/memory.h"
#include "base/platform.h"
#include "base/assert.h"
#include <stdlib.h>

namespace base
{
class MainMemoryAllocator : public MemoryAllocator
{
public:
	MainMemoryAllocator()
	{
		allocationCount = 0;
		allocatedSize = 0;
	}
	
	~MainMemoryAllocator()
	{
	#ifdef _DEBUG
		if (allocationCount != 0)
		{
		#ifdef B_LEAK_FATAL_BREAK
			fatalMessageBox("There were several leaks detected! Please run with a debugger connected.");
			_CrtDbgBreak();
		#endif
		}
	#endif
	}

	void* allocate(size_t size, size_t align = 0)
	{
		++allocationCount;
		allocatedSize += size;
		return malloc(size);
	}
	
	void deallocate(void* addr)
	{
		if (!addr)
		{
			return;
		}

		--allocationCount;
#ifdef _WINDOWS
		allocatedSize -= _msize(addr);
#endif
		free(addr);
	}
	
	size_t getAllocatedSize(void* addr)
	{
		if (!addr)
		{
			return 0;
		}
#ifdef _WINDOWS
		return _msize(addr);
#endif
		return 0;
	}
	
	size_t getAllocationCount()
	{
		return allocationCount;
	}
	
	size_t getTotalAllocatedSize()
	{
		return allocatedSize;
	}

	size_t allocationCount;
	size_t allocatedSize;
};

MainMemoryAllocator* mainAllocator = nullptr;

MemoryAllocator* getMainAllocator()
{
	if (!mainAllocator)
	{
		mainAllocator = (MainMemoryAllocator*)malloc(sizeof(MainMemoryAllocator));
		new (mainAllocator) MainMemoryAllocator();
	}
	
	return mainAllocator;
}

void deleteMainAllocator()
{
	if (mainAllocator)
	{
		mainAllocator->~MainMemoryAllocator();
		mainAllocator = nullptr;
	}
}

}

#ifdef B_USE_CUSTOM_ALLOCATORS
void* operator new(size_t amount)
{
#ifdef B_FORCE_CUSTOM_ALLOCATOR_MACROS
	base::fatalMessageBox("Use the B_NEW in the code, not 'new'. Needed for precise memory leak detection.");
	_CrtDbgBreak();
	exit(0);
	return nullptr;
#endif

	return base::getMainAllocator()->allocate(amount);
}

void* operator new[](size_t amount)
{
#ifdef B_FORCE_CUSTOM_ALLOCATOR_MACROS
	base::fatalMessageBox("Use the B_NEW_ARRAY in the code, not 'new[]'. Needed for precise memory leak detection.");
	_CrtDbgBreak();
	exit(0);
	return nullptr;
#endif

	return base::getMainAllocator()->allocate(amount);
}

void operator delete(void *ptr)
{
#ifdef B_FORCE_CUSTOM_ALLOCATOR_MACROS
	base::fatalMessageBox("Use the B_DELETE in the code, not 'delete'. Needed for precise memory leak detection.");
	_CrtDbgBreak();
	exit(0);
#endif

	base::getMainAllocator()->deallocate(ptr);
}

void operator delete[](void *ptr)
{
#ifdef B_FORCE_CUSTOM_ALLOCATOR_MACROS
	base::fatalMessageBox("Use the B_DELETE_ARRAY in the code, not 'delete[]'. Needed for precise memory leak detection.");
	_CrtDbgBreak();
	exit(0);
#endif

	base::getMainAllocator()->deallocate(ptr);
}
#endif