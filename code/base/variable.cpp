#include <stdio.h>
#include "base/variable.h"
#include "base/math/vec3.h"
#include "base/math/vec2.h"
#include "base/math/color.h"
#include "base/json.h"
#include "base/logger.h"
#include "base/cmdline_arguments.h"

namespace base
{
bool VariableRegistry::registerVariable(
	const String& name,
	void* varAddress,
	VariantType varType,
	Variant defaultValue,
	const String& comment,
	VariableFlags flags,
	Variant minVal,
	Variant maxVal)
{
	AutoLock<Mutex> locker(lock);

	auto iter = variables.find(name);
	Variable var;

	var.type = varType;
	var.name = name;
	var.comment = comment;
	var.variable = varAddress;
	var.flags = flags;
	var.defaultValue = defaultValue;
	var.minValue = minVal;
	var.maxValue = maxVal;

	if (variables.end() == iter)
	{
		variables[name] = var;
	}

	setVariableValueFromText(name, defaultValue.toString());

	return false;
}

void VariableRegistry::unregisterVariable(const String& name)
{
	AutoLock<Mutex> locker(lock);
	auto iter = variables.find(name);

	if (variables.end() == iter)
	{
		return;
	}

	variables.erase(name);
}

Variable* VariableRegistry::getVariable(const String& name)
{
	AutoLock<Mutex> locker(lock);
	auto iter = variables.find(name);

	if (variables.end() == iter)
	{
		B_LOG_ERROR("Unknown variable '" << name << "'");
		return nullptr;
	}

	return &iter->value;
}

bool VariableRegistry::loadVariables(const String& filename)
{
	AutoLock<Mutex> locker(lock);
	JsonDocument doc;

	if (!doc.loadAndParse(filename))
	{
		return false;
	}

	if (doc.hasErrors())
	{
		return false;
	}

	for (size_t i = 0; i < doc.getRoot().getMembers().size(); ++i)
	{
		const auto& pair = doc.getRoot().getMembers().at(i);
		Variable* var = getVariable(pair.key);

		if (var)
		{
			setVariableValueFromText(pair.key, pair.value->toString());
			B_LOG_DEBUG("Set " << pair.key << "=" << pair.value->toString());
		}
	}

	return true;
}

bool VariableRegistry::loadVariables(const CommandLineArguments& args)
{
	// load all vars found in arguments
	for (size_t i = 0; i < args.arguments.size(); i++)
	{
		auto& arg = args.arguments[i];
		auto var = getVariable(arg.key);

		if (var)
		{
			setVariableValueFromText(arg.key, arg.value);
		}
	}

	return true;
}

void VariableRegistry::setVariableValueFromText(const String& name, const String& value)
{
	AutoLock<Mutex> locker(lock);
	Variable* varPtr = getVariable(name);

	if (!varPtr)
	{
		B_LOG_ERROR("Variable name '" << name << "' not found");
		return;
	}

	Variant var;

	var.setType(varPtr->type);
	var.setFromText(value);

	switch (varPtr->type)
	{
	case VariantType::Int8:
		setVariableValue(name, (i8)var);
		break;
	case VariantType::Int16:
		setVariableValue(name, (i16)var);
		break;
	case VariantType::Int32:
		setVariableValue(name, (i32)var);
		break;
	case VariantType::Int64:
		setVariableValue(name, (i64)var);
		break;
	case VariantType::Float:
		setVariableValue(name, (f32)var);
		break;
	case VariantType::Double:
		setVariableValue(name, (f64)var);
		break;
	case VariantType::String:
		setVariableValue(name, (String)var);
		break;
	case VariantType::Bool:
		setVariableValue(name, (bool)var);
		break;
	case VariantType::Vec3:
		setVariableValue(name, (Vec3)var);
		break;
	case VariantType::Color:
		setVariableValue(name, (Color)var);
		break;
	case VariantType::Ptr:
		setVariableValue(name, (void*)var);
		break;
	case VariantType::Quat:
		setVariableValue(name, (Quat)var);
		break;
	case VariantType::Matrix:
		setVariableValue(name, (Matrix)var);
		break;
	default:
		break;
	}
}

}