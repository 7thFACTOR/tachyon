#pragma once
#include <new>
#include "base/assert.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/allocator.h"

namespace base
{
template <typename Type, typename AllocatorType = Allocator>
class Array
{
public:
	typedef Type* Iterator;
	typedef AllocatorType allocatorType;
	typedef bool (*CompareFunction)(Type* a, Type* b);
	static const size_t invalidIndex = ~0;
	
	Array();
	Array(const Array& other);
	Array(size_t size);
	~Array();

	Type& at(size_t index) const;
	inline size_t size() const;
	inline size_t byteSize() const;
	bool isEmpty() const;
	bool notEmpty() const { return elementCount != 0; }
	size_t capacity() const;
	Type* data() const;
	void fill(size_t fromIndex, size_t count, const Type& value);
	void reserve(size_t size);
	void useGrowSize(size_t size);
	size_t getGrowCount() const;
	void resize(size_t size);
	void clear();
	void optimize();
	void append(const Type& elem);
	void prepend(const Type& elem);
	bool popFirst(Type& elem);
	bool popLast(Type& elem);
	bool appendUnique(const Type& elem);
	void appendArray(const Array<Type, AllocatorType>& other);
	void appendArrayUnique(const Array<Type, AllocatorType>& other);
	Type& front() const;
	Type& back() const;
	Iterator begin() const;
	Iterator end() const;
	Iterator erase(Iterator iter);
	bool erase(const Type& elem);
	Iterator eraseSwap(Iterator iter);
	void eraseAt(size_t index);
	void eraseSwapAt(size_t index);
	Iterator find(const Type& elem) const;
	size_t findIndex(const Type& elem) const;
	bool contains(const Type& elem) const;
	size_t binarySearch(const Type& elem) const;
	void insert(size_t index, const Type& elem);
	size_t insertSorted(const Type& elem);
	bool isSorted() const;
	size_t insertAtEndOfIdenticalRange(size_t startIndex, const Type& elem);

	Array& operator = (const Array& other);
	Type& operator [](size_t index);
	const Type& operator [](size_t index) const;

protected:
	void growArray(size_t amount = 0);
	void moveArray(size_t fromIndex, size_t toIndex);
	void deleteArray();
	void copyArray(const Array& src);
	void destroyElement(Type* obj)
	{
		B_ASSERT(obj);
		obj->~Type();
	}

	Type *elements;
	size_t elementCount;
	size_t growCount;
	size_t maxCapacity;
	AllocatorType allocator;
	static const size_t defaultGrowSize = 16;
};

//////////////////////////////////////////////////////////////////////////

template <typename Type, typename AllocatorType>
Array<Type, AllocatorType>::Array()
	: growCount(defaultGrowSize)
	, elementCount(0)
	, maxCapacity(0)
	, elements(nullptr)
{
	B_ASSERT(growCount);
}

template <typename Type, typename AllocatorType>
Array<Type, AllocatorType>::Array(const Array& other)
	: growCount(defaultGrowSize)
	, elementCount(0)
	, maxCapacity(0)
	, elements(nullptr)
{
	*this = other;
	B_ASSERT(growCount);
}

template <typename Type, typename AllocatorType>
Array<Type, AllocatorType>::Array(size_t size)
	: growCount(defaultGrowSize)
	, elementCount(0)
	, maxCapacity(0)
	, elements(nullptr)
{
	resize(size);
	B_ASSERT(growCount);
}

template <typename Type, typename AllocatorType>
Array<Type, AllocatorType>::~Array()
{
	if (elements)
	{
		for (size_t i = 0; i < elementCount; ++i)
		{
			destroyElement(&elements[i]);
		}
	}

	allocator.deallocate(elements, maxCapacity * sizeof(Type));
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::deleteArray()
{
	if (elements)
	{
		for (size_t i = 0; i < elementCount; ++i)
		{
			destroyElement(&elements[i]);
		}

		allocator.deallocate(elements, maxCapacity * sizeof(Type));
		elements = nullptr;
	}
}

template<typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::copyArray(const Array<Type, AllocatorType>& src)
{
	size_t oldCount = elementCount;
	size_t oldMaxCapacity = maxCapacity;
	growCount = src.growCount;
	maxCapacity = src.maxCapacity;
	elementCount = src.elementCount;

	if (maxCapacity > 0)
	{
		u8* newBuffer = (u8*)allocator.allocate(maxCapacity * sizeof(Type));

		if (elements)
		{
			for (size_t i = 0; i < oldCount; ++i)
			{
				destroyElement(&elements[i]);
			}
		}

		allocator.deallocate(elements, oldMaxCapacity * sizeof(Type));
		elements = (Type*)newBuffer;

		for (size_t i = 0; i < elementCount; ++i)
		{
			new (&elements[i]) Type(src.elements[i]);
		}
	}
	else
	{
		allocator.deallocate(elements, oldMaxCapacity * sizeof(Type));
		elements = nullptr;
		elementCount = 0;
	}
}

template <typename Type, typename AllocatorType>
Type& Array<Type, AllocatorType>::at(size_t index) const
{
	B_ASSERT(B_INBOUNDS(index, 0, elementCount));
	B_ASSERT(elements);
	return elements[index];
}

template <typename Type, typename AllocatorType>
inline size_t Array<Type, AllocatorType>::size() const
{
	return elementCount;
}

template <typename Type, typename AllocatorType>
inline size_t Array<Type, AllocatorType>::byteSize() const
{
	return elementCount * sizeof(Type);
}

template <typename Type, typename AllocatorType>
bool Array<Type, AllocatorType>::isEmpty() const
{
	return elementCount == 0;
}

template <typename Type, typename AllocatorType>
size_t Array<Type, AllocatorType>::capacity() const
{
	return maxCapacity;
}

template <typename Type, typename AllocatorType>
Type* Array<Type, AllocatorType>::data() const
{
	return elements;
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::clear()
{
	if (elements)
	{
		for (size_t i = 0; i < elementCount; ++i)
		{
			destroyElement(&elements[i]);
		}
	}

	elementCount = 0;
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::fill(size_t fromIndex, size_t count, const Type& value)
{
	B_ASSERT(B_INBOUNDS(fromIndex, 0, elementCount));
	B_ASSERT(elements);

	for (size_t i = fromIndex, iTo = fromIndex + count; i < iTo; ++i)
	{
		elements[i] = value;
	}
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::reserve(size_t size)
{
	if (size <= maxCapacity)
	{
		return;
	}

	u8* newBuffer = (u8*)allocator.allocate(size * sizeof(Type));
	Type* newArray = (Type*)newBuffer;

	B_ASSERT(newArray);

	for (size_t i = 0; i < elementCount; ++i)
	{
		new (&newArray[i]) Type(elements[i]);
	}

	if (elements)
	{
		for (size_t i = 0; i < elementCount; ++i)
		{
			destroyElement(&elements[i]);
		}
	}

	allocator.deallocate(elements, maxCapacity * sizeof(Type));
	elements = newArray;
	maxCapacity = size;
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::useGrowSize(size_t size)
{
	growCount = size;
}

template <typename Type, typename AllocatorType>
size_t Array<Type, AllocatorType>::getGrowCount() const
{
	return growCount;
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::resize(size_t size)
{
	if (size == 0)
	{
		if (elements)
		{
			for (size_t i = 0; i < elementCount; ++i)
			{
				destroyElement(&elements[i]);
			}
		}

		elementCount = 0;
		return;
	}

	if (maxCapacity >= size)
	{
		if (size > elementCount)
		{
			for (size_t i = elementCount; i < size; ++i)
			{
				new (&elements[i]) Type();
			}
		}
		else
		{
			if (elements)
			{
				for (size_t i = size; i < elementCount; ++i)
				{
					destroyElement(&elements[i]);
				}
			}
		}

		elementCount = size;
		return;
	}

	u8* newBuffer = (u8*)allocator.allocate(size * sizeof(Type));
	Type* newArray = (Type*)newBuffer;
	B_ASSERT(newArray);

	for (size_t i = 0; i < elementCount; ++i)
	{
		new (&newArray[i]) Type(elements[i]);
	}

	for (size_t i = elementCount; i < size; ++i)
	{
		new (&newArray[i]) Type();
	}

	if (elements)
	{
		for (size_t i = 0; i < elementCount; ++i)
		{
			destroyElement(&elements[i]);
		}
	}

	allocator.deallocate(elements, maxCapacity *sizeof(Type));
	elements = newArray;
	elementCount = maxCapacity = size;
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::optimize()
{
	if (elementCount > 0)
	{
		u8* newBuffer = (u8*)allocator.allocate(elementCount * sizeof(Type));
		Type* newArray = (Type*)newBuffer;
		B_ASSERT(newArray);

		if (elements)
		{
			for (size_t i = 0; i < elementCount; ++i)
			{
				new (&newArray[i]) Type(elements[i]);
				destroyElement(&elements[i]);
			}
		}

		allocator.deallocate(elements, maxCapacity * sizeof(Type));
		elements = newArray;
		maxCapacity = elementCount;
	}
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::append(const Type& elem)
{
	if (elementCount + 1 > maxCapacity)
	{
		growArray();
	}

	if (!elements)
	{
		B_ASSERT(!"No elements array created!");
		return;
	}

	new (&elements[elementCount]) Type(elem);
	++elementCount;
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::prepend(const Type& elem)
{
	insert(0, elem);
}

template <typename Type, typename AllocatorType>
bool Array<Type, AllocatorType>::appendUnique(const Type& elem)
{
	if (findIndex(elem) != -1)
	{
		return false;
	}

	insertSorted(elem);

	return true;
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::appendArray(const Array<Type, AllocatorType>& other)
{
	if (elementCount + other.elementCount > maxCapacity)
	{
		growArray(other.elementCount);
	}

	for (size_t i = 0, iCount = other.elementCount; i < iCount; ++i)
	{
		new (&elements[elementCount + i]) Type(other.elements[i]);
	}

	elementCount += other.elementCount;
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::appendArrayUnique(const Array<Type, AllocatorType>& other)
{
	for (auto& item : other)
	{
		appendUnique(item);
	}
}

template <typename Type, typename AllocatorType>
bool Array<Type, AllocatorType>::popFirst(Type& elem)
{
	if (!elementCount)
	{
		return false;
	}

	elem = elements[0];
	eraseAt(0);

	return true;
}

template <typename Type, typename AllocatorType>
bool Array<Type, AllocatorType>::popLast(Type& elem)
{
	if (!elementCount)
	{
		return false;
	}

	elem = elements[elementCount - 1];
	eraseAt(elementCount - 1);

	return true;
}

template <typename Type, typename AllocatorType>
Type& Array<Type, AllocatorType>::front() const
{
	B_ASSERT(elements && elementCount);
	return elements[0];
}

template <typename Type, typename AllocatorType>
Type& Array<Type, AllocatorType>::back() const
{
	B_ASSERT(elements && elementCount);
	return elements[elementCount - 1];
}

template <typename Type, typename AllocatorType>
typename Array<Type, AllocatorType>::Iterator Array<Type, AllocatorType>::begin() const
{
	return elements;
}

template <typename Type, typename AllocatorType>
typename Array<Type, AllocatorType>::Iterator Array<Type, AllocatorType>::end() const
{
	return elements + elementCount;
}

template <typename Type, typename AllocatorType>
typename Array<Type, AllocatorType>::Iterator Array<Type, AllocatorType>::erase(Iterator iter)
{
	eraseAt(size_t(iter - elements));
	return iter;
}

template <typename Type, typename AllocatorType>
bool Array<Type, AllocatorType>::erase(const Type& elem)
{
	size_t index = findIndex(elem);

	if (index == invalidIndex)
	{
		return false;
	}

	eraseAt(index);

	return true;
}

template <typename Type, typename AllocatorType>
typename Array<Type, AllocatorType>::Iterator Array<Type, AllocatorType>::eraseSwap(Iterator iter)
{
	eraseSwapAt(size_t(iter - elements));
	return iter;
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::eraseAt(size_t index)
{
	B_ASSERT(elements && (index < elementCount));
	
	if (index == (elementCount - 1))
	{
		destroyElement(&(elements[index]));
		--elementCount;
	}
	else
	{
		moveArray(index + 1, index);
	}
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::eraseSwapAt(size_t index)
{
	B_ASSERT(elements && (index < elementCount));
	size_t lastIndex = elementCount - 1;

	if (index < lastIndex)
	{
		elements[index] = elements[lastIndex];
	}

	destroyElement(&(elements[lastIndex]));
	--elementCount;
}

template <typename Type, typename AllocatorType>
typename Array<Type, AllocatorType>::Iterator Array<Type, AllocatorType>::find(const Type& elem) const
{
	for (size_t i = 0; i < elementCount; ++i)
	{
		if (elements[i] == elem)
		{
			return &(elements[i]);
		}
	}

	return end();
}

template <typename Type, typename AllocatorType>
size_t Array<Type, AllocatorType>::findIndex(const Type& elem) const
{
	for (size_t i = 0; i < elementCount; ++i)
	{
		if (elements[i] == elem)
		{
			return i;
		}
	}
	return -1;
}

template <typename Type, typename AllocatorType>
bool Array<Type, AllocatorType>::contains(const Type& elem) const
{
	return findIndex(elem) != -1;
}

template <typename Type, typename AllocatorType>
size_t Array<Type, AllocatorType>::binarySearch(const Type& elem) const
{
	size_t num = elementCount;
	
	if (num > 0)
	{
		size_t half;
		size_t lo = 0;
		size_t hi = num - 1;
		size_t mid;

		while (lo <= hi) 
		{
			if (0 != (half = num / 2)) 
			{
				mid = lo + ((num & 1) ? half : (half - 1));

				if (elem < elements[mid])
				{
					hi = mid - 1;
					num = num & 1 ? half : half - 1;
				} 
				else if (elem > elements[mid]) 
				{
					lo = mid + 1;
					num = half;
				} 
				else
				{
					return mid;
				}
			} 
			else if (0 != num) 
			{
				if (elem != elements[lo])
				{
					return -1;
				}
				else
				{
					return lo;
				}
			} 
			else 
			{
				break;
			}
		}
	}

	return -1;
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::insert(size_t index, const Type& elem)
{
	B_ASSERT(index <= elementCount);
	
	if (index == elementCount)
	{
		append(elem);
	}
	else
	{
		moveArray(index, index + 1);
		elements[index] = elem;
	}
}

template <typename Type, typename AllocatorType>
size_t Array<Type, AllocatorType>::insertSorted(const Type& elem)
{
	size_t num = elementCount;

	if (num == 0)
	{
		append(elem);
		return elementCount - 1;
	}
	else
	{
		size_t half;
		size_t lo = 0;
		size_t hi = num - 1;
		size_t mid;

		while (lo <= hi) 
		{
			if (0 != (half = num / 2)) 
			{
				mid = lo + ((num & 1) ? half : (half - 1));
				
				if (elem < elements[mid])
				{
					hi = mid - 1;
					num = num & 1 ? half : half - 1;
				} 
				else if (elem > elements[mid]) 
				{
					lo = mid + 1;
					num = half;
				} 
				else
				{
					return insertAtEndOfIdenticalRange(mid, elem);
				}
			} 
			else if (0 != num) 
			{
				if (elem < elements[lo])
				{
					insert(lo, elem);
					return lo;
				}
				else if (elem > elements[lo])
				{
					insert(lo + 1, elem);
					return lo + 1;
				}
				else
				{
					return insertAtEndOfIdenticalRange(lo, elem);
				}
			} 
			else 
			{
				B_ASSERT(0 == lo);
				insert(lo, elem);
				return lo;
			}
		}
		if (elem < elements[lo])
		{
			insert(lo, elem);
			return lo;
		}
		else if (elem > elements[lo])
		{
			insert(lo + 1, elem);
			return lo + 1;
		}
	}

	return -1;
}

template <typename Type, typename AllocatorType>
bool Array<Type, AllocatorType>::isSorted() const
{
	if (elementCount > 1)
	{
		for (size_t i = 0; i < elementCount - 1; ++i)
		{
			if (elements[i] > elements[i + 1])
			{
				return false;
			}
		}
	}
	
	return true;
}

template <typename Type, typename AllocatorType>
Array<Type, AllocatorType>& Array<Type, AllocatorType>::operator = (const Array& other)
{
	if (this != &other)
	{
		if ((maxCapacity > 0) && (other.elementCount <= maxCapacity))
		{
			B_ASSERT(elements);
			size_t i;

			for (i = elementCount; i < other.elementCount; ++i)
			{
				new (&elements[i]) Type();
			}

			for (i = 0; i < other.elementCount; ++i)
			{
				elements[i] = other.elements[i];
			}

			// destroy leftovers
			for (; i < elementCount; ++i)
			{
				destroyElement(&(elements[i]));
			}

			growCount = other.growCount;
			elementCount = other.elementCount;
		}
		else
		{
			deleteArray();
			copyArray(other);
		}
	}

	return *this;
}

template <typename Type, typename AllocatorType>
Type& Array<Type, AllocatorType>::operator [](size_t index)
{
	return elements[index];
}

template <typename Type, typename AllocatorType>
const Type& Array<Type, AllocatorType>::operator [](size_t index) const
{
	return elements[index];
}

template <typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::moveArray(size_t fromIndex, size_t toIndex)
{
	B_ASSERT(elements);
	B_ASSERT(fromIndex < elementCount);

	if (fromIndex == toIndex)
	{
		return;
	}

	size_t moveCount = elementCount - fromIndex;
	size_t neededSize = toIndex + moveCount;

	while (neededSize > maxCapacity)
	{
		growArray();
	}

	if (fromIndex > toIndex)
	{
		size_t i;

		for (i = 0; i < moveCount; ++i)
		{
			if (toIndex + i < elementCount)
			{
				elements[toIndex + i] = elements[fromIndex + i];
			}
			else
			{
				new (&elements[toIndex + i]) Type(elements[fromIndex + i]);
			}
		}

		if (elements)
		{
			for (i = (fromIndex + i) - 1; i < elementCount; ++i)
			{
				destroyElement(&(elements[i]));
				new (&elements[i]) Type();
			}
		}
	}
	else
	{
		int i;

		for (i = (int)moveCount - 1; i >= 0; --i)
		{
			if (toIndex + i < elementCount)
			{
				elements[toIndex + i] = elements[fromIndex + i];
			}
			else
			{
				new (&elements[toIndex + i]) Type(elements[fromIndex + i]);
			}
		}

		if (elements)
		{
			for (i = int(fromIndex); i < int(toIndex); ++i)
			{
				destroyElement(&(elements[i]));
				new (&elements[i]) Type();
			}
		}
	}

	elementCount = toIndex + moveCount;
}

template<typename Type, typename AllocatorType>
size_t Array<Type, AllocatorType>::insertAtEndOfIdenticalRange(size_t startIndex, const Type& elem)
{
	size_t i = startIndex + 1;

	for (; i < elementCount; ++i)
	{
		if (elements[i] != elem)
		{
			insert(i, elem);
			return i;
		}
	}

	append(elem);
	return (elementCount - 1);
}

template<typename Type, typename AllocatorType>
void Array<Type, AllocatorType>::growArray(size_t amount)
{
	B_ASSERT(growCount);
	size_t newCapacity = maxCapacity + (amount != 0 ? amount : growCount);
	B_ASSERT(newCapacity);
	
	u8* newBuffer = (u8*)allocator.allocate(newCapacity * sizeof(Type));
	Type* newArray = (Type*)newBuffer;

	for (size_t i = 0; i < elementCount; ++i)
	{
		new (&newArray[i]) Type(elements[i]);
	}

	if (elements)
	{
		for (size_t i = 0; i < elementCount; ++i)
		{
			destroyElement(&elements[i]);
		}
	}

	allocator.deallocate(elements, maxCapacity * sizeof(Type));
	elements = newArray;
	maxCapacity = newCapacity;
}

}