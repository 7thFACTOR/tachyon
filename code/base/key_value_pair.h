// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once

namespace base
{
template<typename KeyType, typename ValueType>
class KeyValuePair
{
public:
	KeyValuePair();
	KeyValuePair(const KeyType& key, const ValueType& value);
	KeyValuePair(const KeyType& key);
	KeyValuePair(const KeyValuePair<KeyType, ValueType>& other);
	
	void operator = (const KeyValuePair<KeyType, ValueType>& other);
	bool operator == (const KeyValuePair<KeyType, ValueType>& other) const;
	bool operator != (const KeyValuePair<KeyType, ValueType>& other) const;
	bool operator > (const KeyValuePair<KeyType, ValueType>& other) const;
	bool operator >= (const KeyValuePair<KeyType, ValueType>& other) const;
	bool operator < (const KeyValuePair<KeyType, ValueType>& other) const;
	bool operator <= (const KeyValuePair<KeyType, ValueType>& other) const;
	
	KeyType key;
	ValueType value;
};

template<typename KeyType, typename ValueType>
KeyValuePair<KeyType, ValueType>::KeyValuePair()
{}

template<typename KeyType, typename ValueType>
KeyValuePair<KeyType, ValueType>::KeyValuePair(const KeyType& newKey, const ValueType& newValue)
{
	key = newKey;
	value = newValue;
}

template<typename KeyType, typename ValueType>
KeyValuePair<KeyType, ValueType>::KeyValuePair(const KeyType& newKey)
{
	key = newKey;
}

template<typename KeyType, typename ValueType>
KeyValuePair<KeyType, ValueType>::KeyValuePair(const KeyValuePair<KeyType, ValueType>& other)
	: key(other.key)
	, value(other.value)
{}

template<typename KeyType, typename ValueType>
void KeyValuePair<KeyType, ValueType>::operator = (const KeyValuePair<KeyType, ValueType>& other)
{
	key = other.key;
	value = other.value;
}

template<typename KeyType, typename ValueType>
bool KeyValuePair<KeyType, ValueType>::operator == (const KeyValuePair<KeyType, ValueType>& other) const
{
	return (key == other.key);
}

template<typename KeyType, typename ValueType>
bool KeyValuePair<KeyType, ValueType>::operator != (const KeyValuePair<KeyType, ValueType>& other) const
{
	return (key != other.key);
}

template<typename KeyType, typename ValueType>
bool KeyValuePair<KeyType, ValueType>::operator > (const KeyValuePair<KeyType, ValueType>& other) const
{
	return (key > other.key);
}

template<typename KeyType, typename ValueType>
bool KeyValuePair<KeyType, ValueType>::operator >= (const KeyValuePair<KeyType, ValueType>& other) const
{
	return (key >= other.key);
}

template<typename KeyType, typename ValueType>
bool KeyValuePair<KeyType, ValueType>::operator < (const KeyValuePair<KeyType, ValueType>& other) const
{
	return (key < other.key);
}

template<typename KeyType, typename ValueType>
bool KeyValuePair<KeyType, ValueType>::operator <= (const KeyValuePair<KeyType, ValueType>& other) const
{
	return (key <= other.key);
}

}