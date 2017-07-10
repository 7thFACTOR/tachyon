#pragma once
#include "base/array.h"
#include "base/key_value_pair.h"

namespace base
{
const size_t invalidDictionaryIndex = ~0;

//TODO: add Allocator support
//! A very simple array of key-value pairs, this is not a hash map
template<typename KeyType, typename ValueType>
class Dictionary
{
public:
	typedef KeyValuePair<KeyType, ValueType> PairType;
	typedef PairType* Iterator;

	Dictionary();
	Dictionary(const Dictionary<KeyType, ValueType>& other);
	void operator = (const Dictionary<KeyType, ValueType>& other);
	ValueType& operator[](const KeyType& key);
	ValueType& operator[](KeyType& key);

	size_t size() const;
	void clear();
	bool isEmpty() const;
	void reserve(size_t count);
	KeyValuePair<KeyType, ValueType>& front() const;
	KeyValuePair<KeyType, ValueType>& back() const;
	Iterator begin() const;
	Iterator end() const;
	Iterator erase(Iterator iter);
	void beginBulkAdd();
	void add(const KeyValuePair<KeyType, ValueType>& pair);
	void add(const KeyType& key, const ValueType& value);
	void endBulkAdd();
	void erase(const KeyType& key);
	void eraseAt(size_t index);
	size_t findIndex(const KeyType& key) const;
	Iterator find(const KeyType& key) const;
	bool contains(const KeyType& key) const;
	const KeyType& keyAt(size_t index) const;
	ValueType& valueAt(size_t index);
	const ValueType& valueAt(size_t index) const;
	KeyValuePair<KeyType, ValueType>& keyValuePairAt(size_t index) const;
	Array<KeyType> keysAsArray() const;
	Array<ValueType> valuesAsArray() const;
	template<typename _TyReturn> _TyReturn keysAs() const;
	template<typename _TyReturn> _TyReturn valuesAs() const;

protected:
	Array<KeyValuePair<KeyType, ValueType> > items;
	bool bulkInsert = false;
};

//////////////////////////////////////////////////////////////////////////

template<typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::Dictionary()
	: bulkInsert(false)
{
	B_ASSERT(items.getGrowCount());
}

template<typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::Dictionary(const Dictionary<KeyType, ValueType>& other)
	: items(other.items)
	, bulkInsert(false)
{
	B_ASSERT(items.getGrowCount());
	B_ASSERT(!other.bulkInsert);
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::operator = (const Dictionary<KeyType, ValueType>& other)
{
	B_ASSERT(!bulkInsert);
	B_ASSERT(!other.bulkInsert);
	items = other.items;
	B_ASSERT(items.getGrowCount());
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::clear()
{
	B_ASSERT(!bulkInsert);
	items.clear();
}

template<typename KeyType, typename ValueType>
size_t Dictionary<KeyType, ValueType>::size() const
{
	return items.size();
}

template<typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::isEmpty() const
{
	return (0 == items.size());
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::add(const KeyValuePair<KeyType, ValueType>& pair)
{
	B_ASSERT(find(pair.key) == end());

	if (bulkInsert)
	{
		items.append(pair);
	}
	else
	{
		items.insertSorted(pair);
	}
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::reserve(size_t count)
{
	items.reserve(count);
}

template<typename KeyType, typename ValueType>
KeyValuePair<KeyType, ValueType>& Dictionary<KeyType, ValueType>::front() const
{
	B_ASSERT(!items.isEmpty());
	return items.front();
}

template<typename KeyType, typename ValueType>
KeyValuePair<KeyType, ValueType>& Dictionary<KeyType, ValueType>::back() const
{
	B_ASSERT(!items.isEmpty());
	return items.back();
}

template<typename KeyType, typename ValueType>
typename Dictionary<KeyType, ValueType>::Iterator Dictionary<KeyType, ValueType>::begin() const
{
	return items.begin();
}

template<typename KeyType, typename ValueType>
typename Dictionary<KeyType, ValueType>::Iterator Dictionary<KeyType, ValueType>::end() const
{
	return items.end();
}

template<typename KeyType, typename ValueType>
typename Dictionary<KeyType, ValueType>::Iterator Dictionary<KeyType, ValueType>::erase(Iterator iter)
{
	return items.erase(iter);
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::beginBulkAdd()
{
	B_ASSERT(!bulkInsert);
	bulkInsert = true;
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::endBulkAdd()
{
	B_ASSERT(bulkInsert);
	items.sort();
	bulkInsert = false;
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::add(const KeyType& key, const ValueType& value)
{
	KeyValuePair<KeyType, ValueType> pair(key, value);

	B_ASSERT(find(pair.key) == end());

	if (bulkInsert)
	{
		items.append(pair);
	}
	else
	{
		items.insertSorted(pair);
	}
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::erase(const KeyType& key)
{
	B_ASSERT(!bulkInsert);
	size_t eraseIndex = items.binarySearch(key);
	B_ASSERT(invalidDictionaryIndex != eraseIndex);

	if (invalidDictionaryIndex != eraseIndex)
	{
		items.eraseAt(eraseIndex);
	}
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::eraseAt(size_t index)
{
	B_ASSERT(!bulkInsert);
	B_ASSERT(B_INBOUNDS(index, 0, items.size()));
	items.eraseAt(index);
}

template<typename KeyType, typename ValueType>
size_t Dictionary<KeyType, ValueType>::findIndex(const KeyType& key) const
{
	B_ASSERT(!bulkInsert);
	return items.binarySearch(key);
}

template<typename KeyType, typename ValueType>
typename Dictionary<KeyType, ValueType>::Iterator Dictionary<KeyType, ValueType>::find(const KeyType& key) const
{
	size_t index = items.binarySearch(key);
	
	if (index == invalidDictionaryIndex)
	{
		return end();
	}

	return &items.at(index);
}

template<typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::contains(const KeyType& key) const
{
	B_ASSERT(!bulkInsert);
	return (invalidDictionaryIndex != items.binarySearch(key));
}

template<typename KeyType, typename ValueType>
const KeyType& Dictionary<KeyType, ValueType>::keyAt(size_t index) const
{
	B_ASSERT(!bulkInsert);
	return items[index].key;
}

template<typename KeyType, typename ValueType>
ValueType& Dictionary<KeyType, ValueType>::valueAt(size_t index)
{
	B_ASSERT(!bulkInsert);
	return items[index].value;
}

template<typename KeyType, typename ValueType>
const ValueType& Dictionary<KeyType, ValueType>::valueAt(size_t index) const
{
	B_ASSERT(!bulkInsert);
	return items[index].value;
}

template<typename KeyType, typename ValueType>
KeyValuePair<KeyType, ValueType>& Dictionary<KeyType, ValueType>::keyValuePairAt(size_t index) const
{
	B_ASSERT(!bulkInsert);
	return items[index];
}

template<typename KeyType, typename ValueType>
ValueType& Dictionary<KeyType, ValueType>::operator [] (const KeyType& key)
{
	B_ASSERT(items.getGrowCount());
	size_t keyValuePairIndex = findIndex(key);

	if (invalidDictionaryIndex == keyValuePairIndex)
	{
		add(key, ValueType());
		keyValuePairIndex = findIndex(key);
	}

	B_ASSERT(keyValuePairIndex != invalidDictionaryIndex);
	
	return items[keyValuePairIndex].value;
}

//TODO: use && move op
template<typename KeyType, typename ValueType>
ValueType& Dictionary<KeyType, ValueType>::operator [] (KeyType& key)
{
	B_ASSERT(items.getGrowCount());
	size_t keyValuePairIndex = findIndex(key);

	if (invalidDictionaryIndex == keyValuePairIndex)
	{
		add(key, ValueType());
		keyValuePairIndex = findIndex(key);
	}

	B_ASSERT(keyValuePairIndex != invalidDictionaryIndex);

	return items[keyValuePairIndex].value;
}

template<typename KeyType, typename ValueType>
template<typename _ReturnType>
_ReturnType Dictionary<KeyType, ValueType>::valuesAs() const
{
	B_ASSERT(!bulkInsert);
	_ReturnType result(size(), size());

	for (size_t i = 0, iCount = items.size(); i < iCount; ++i)
	{
		result.append(items[i].value);
	}
	
	return result;
}

template<typename KeyType, typename ValueType>
Array<ValueType> Dictionary<KeyType, ValueType>::valuesAsArray() const
{
	return valuesAs<Array<ValueType> >();
}

template<typename KeyType, typename ValueType>
template<typename _ReturnType>
_ReturnType Dictionary<KeyType, ValueType>::keysAs() const
{
	B_ASSERT(!bulkInsert);
	_ReturnType result(size(), size());

	for (size_t i = 0, iCount = items.size(); i < iCount; ++i)
	{
		result.append(items[i].key);
	}

	return result;
}

template<typename KeyType, typename ValueType>
Array<KeyType> Dictionary<KeyType, ValueType>::keysAsArray() const
{
	return keysAs<Array<KeyType> >();
}

}