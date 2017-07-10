#pragma once
#include "base/dictionary.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/variant.h"
#include "base/observer.h"
#include "base/logger.h"
#include "base/mutex.h"

namespace base
{
enum class VariableFlags
{
	None,
	ReadOnly = B_BIT(0),
	DevMode = B_BIT(1),
	CheckRange = B_BIT(2)
};
B_ENUM_AS_FLAGS(VariableFlags);

struct Variable
{
	String name, comment;
	VariableFlags flags;
	void* variable = nullptr;
	VariantType type = VariantType::None;
	Variant defaultValue, minValue, maxValue;
};

class VariableObserver
{
public:
	virtual void onVariableValueChanged(Variable* var) = 0;
};

class B_API VariableRegistry : public Observable<VariableObserver>
{
public:
	bool registerVariable(
		const String& name,
		void* varAddress,
		VariantType varType,
		Variant defaultValue,
		const String& comment = "",
		VariableFlags flags = VariableFlags::None,
		Variant minVal = 0,
		Variant maxVal = 0);
	void unregisterVariable(const String& name);
	Variable* getVariable(const String& name);
	const Dictionary<String, Variable>& getVariables() const { return variables; }
	template<typename Type>
	Type getVariableValue(const String& name)
	{
		Variable* var = getVariable(name);

		if (var)
		{
			return *((Type*)var->variable);
		}
		else
		{
			B_LOG_ERROR("Cannot get unknown variable '" << name << "'. Please register it first.");
		}

		return Type();
	}

	template<typename Type>
	void setVariableValue(const String& name, const Type& value)
	{
		AutoLock<Mutex> locker(lock);
		Variable* var = getVariable(name);

		if (var)
		{
			*((Type*)var->variable) = value;
			return;
		}
		else
		{
			B_LOG_ERROR("Cannot set unknown variable '" << name << "'. Please register it first.");
		}
	}

	void setVariableValueFromText(const String& name, const String& value);
	bool loadVariables(const String& filename);
	bool loadVariables(const class CommandLineArguments& args);

protected:
	Mutex lock;
	Dictionary<String, Variable> variables;
};

}