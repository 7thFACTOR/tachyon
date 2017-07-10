#pragma once
#include "base/array.h"
#include "base/defines.h"

namespace base
{
template<typename Type>
class Observable
{
public:
	typedef Type ThisObserverType;

	Observable();
	virtual ~Observable();

	bool addObserver(Type* obs);
	bool removeObserver(Type* obs);
	void removeObservers();
	inline const Array<Type*>& getObservers() const { return observers; }

protected:
	Array<Type*> observers;
};

template<typename Type>
Observable<Type>::Observable() {}

template<typename Type>
Observable<Type>::~Observable() {}

template<typename Type>
bool Observable<Type>::addObserver(Type* obs)
{
	if (observers.end() != observers.find(obs))
	{
		return false;
	}

	observers.append(obs);

	return true;
}

template<typename Type>
bool Observable<Type>::removeObserver(Type* obs)
{
	for (auto observer : observers)
	{
		if (observer == obs)
		{
			observers.erase(obs);
			return true;
		}
	}

	return false;
}

template<typename Type>
void Observable<Type>::removeObservers()
{
	observers.clear();
}

//! shortcut macro to call all registered observers' method, of an Observable derived class\n
//! Example: B_CALL_OBSERVERS_METHOD(MyObserver, onUpdate(param1, param2));
#define B_CALL_OBSERVERS_METHOD(methodCall)\
	{ for (auto observer : Observable<ThisObserverType>::observers) { observer->methodCall; }	}

//! shortcut macro to call all registered observers' method, of an Observable derived class\n
//! can be used for calls outside the observable class
//! Example: B_CALL_OBSERVERS_METHOD_OF(myObservableObjectPtr, MyObserver, onUpdate(param1, param2));

#define B_CALL_OBSERVERS_METHOD_OF(observable, methodCall)\
	{ for (auto observer : (observable)->Observable<ThisObserverType>::getObservers()) { observer->methodCall; } }

}