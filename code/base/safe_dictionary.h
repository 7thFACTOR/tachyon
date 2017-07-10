#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/assert.h"
#include "base/dictionary.h"
#include "base/mutex.h"

namespace base
{
template <typename KeyType, typename ValueType>
class SafeDictionary : public Dictionary<KeyType, ValueType>
{
public:
	inline Mutex& getMutex() { return safeDictionaryLock; }

protected:
	Mutex safeDictionaryLock;
};

}