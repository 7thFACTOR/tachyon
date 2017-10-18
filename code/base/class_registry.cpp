// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <string.h>
#include "base/class_registry.h"
#include "base/logger.h"

namespace base
{
RuntimeClass* ClassRegistry::newClassInstance(const String& className)
{
	RuntimeClass *obj = nullptr;
	ClassDescriptor* classDesc = nullptr;

	for (auto classDesc : classes)
	{
		if (classDesc->getClassName() == className)
		{
			obj = classDesc->newInstance();
			return obj;
		}
	}

	B_LOG_ERROR(B_TEXT("Could not find a class named: '") << className << B_TEXT("'"));

	return nullptr;
}

void ClassRegistry::registerClass(ClassDescriptor* classDesc)
{
	if (!classDesc)
	{
		B_LOG_ERROR(B_TEXT("nullptr class descriptor given!"));
		return;
	}

	// first see if it is already registered
	for (auto classDescriptor : classes)
	{
		if (classDescriptor == classDesc || classDescriptor->getClassName() == classDesc->getClassName())
		{
			B_LOG_WARNING(B_TEXT("Class '") << classDesc->getClassName() << B_TEXT("' already registered"));
			return;
		}
	}

	classes.append(classDesc);
	B_LOG_INFO(B_TEXT("Registered runtime class: '") << classDesc->getClassName());
}

void ClassRegistry::unregisterClass(ClassDescriptor* classDesc)
{
	auto iter = classes.find(classDesc);

	if (iter == classes.end())
	{
		if (classDesc)
		{
			B_LOG_WARNING(
				B_TEXT("Could not find the given class descriptor: '") << classDesc->getClassName() << B_TEXT("'"));
		}

		return;
	}

	classes.erase(iter);
}

ClassDescriptor* ClassRegistry::findClass(const String& className)
{
	for (auto classDesc : classes)
	{
		if (classDesc->getClassName() == className)
		{
			return classDesc;
		}
	}

	B_LOG_WARNING(B_TEXT("Could not find runtime class descriptor for: '") << className << B_TEXT("'"));
	
	return nullptr;
}

size_t ClassRegistry::findDerivedClasses(const String& baseClassName, Array<ClassDescriptor*>& outClasses)
{
	bool add = false;

	for (auto classDesc : classes)
	{
		if (classDesc->getBaseClassName() == baseClassName)
		{
			outClasses.append(classDesc);
		}
	}

	return outClasses.size();
}

ClassDescriptor* ClassRegistry::findFirstDerivedClass(const String& baseClassName)
{
	for (auto classDesc : classes)
	{
		if (classDesc->getBaseClassName() == baseClassName)
		{
			B_LOG_INFO(B_TEXT("Found first class '") << classDesc->getClassName() << B_TEXT("' derived from '") << baseClassName << B_TEXT("'"));

			return classDesc;
		}
	}

	B_LOG_WARNING(B_TEXT("Could not find a first class descriptor for base class: '") << baseClassName << B_TEXT("' from ") << (int)classes.size() << B_TEXT(" classes"));
	
	return nullptr;
}

}