#include <string.h>
#include "base/rtti.h"
#include "base/logger.h"

namespace base
{
static Array<IRuntimeClass*> s_runtimeClasses;

bool Rtti::copyFrom(Rtti* pFrom)
{
	return false;
}

bool IRuntimeClass::is(const char* pClassName)
{
	return !strcmp(className(), pClassName);
}

bool IRuntimeClass::derivedFrom(const char* pBaseClassName)
{
	return !strcmp(baseClassName(), pBaseClassName);
}

//---

Rtti* newClassInstance(const char* pClassName)
{
	Rtti *pObj = nullptr;
	IRuntimeClass* pClassDesc = nullptr;

	for (u32 i = 0, iCount = s_runtimeClasses.size(); i < iCount; ++i)
	{
		pClassDesc = s_runtimeClasses[i];

		if (!strcmp(pClassDesc->className(), pClassName))
		{
			pObj = (Rtti*)pClassDesc->newInstance();
			return pObj;
		}
	}

	B_LOG_ERROR("Could not find a runtime class named: '%s'", pClassName);

	return nullptr;
}

void registerRuntimeClass(IRuntimeClass* pClassDesc)
{
	if (!pClassDesc)
	{
		B_LOG_ERROR("nullptr runtime class descriptor given!");
		return;
	}

	// first see if it is already registered
	for (size_t i = 0, iCount = s_runtimeClasses.size(); i < iCount; ++i)
	{
		if (s_runtimeClasses[i] == pClassDesc || !strcmp(
					s_runtimeClasses[i]->className(),
					pClassDesc->className()))
		{
			B_LOG_WARNING("Runtime class '%s' already registered", pClassDesc->className());
			return;
		}
	}

	B_LOG_INFO("Registered runtime class: '%s'", pClassDesc->className());
	s_runtimeClasses.append(pClassDesc);
}

void unregisterRuntimeClass(IRuntimeClass* pClassDesc)
{
	Array<IRuntimeClass*>::Iterator iter =
		s_runtimeClasses.find(pClassDesc);

	if(iter == s_runtimeClasses.end())
	{
		if (pClassDesc && Logger::isLoggerSet())
		{
			B_LOG_WARNING(
				"Could not find the given runtime class descriptor: '%s'",
				pClassDesc->className());
		}
		return;
	}

	s_runtimeClasses.erase(iter);
}

IRuntimeClass* findRuntimeClass(const char* pClassName)
{
	for (size_t i = 0, iCount = s_runtimeClasses.size(); i < iCount; ++i)
	{
		if (!strcmp(s_runtimeClasses[i]->className(), pClassName))
		{
			return s_runtimeClasses[i];
		}
	}

	B_LOG_WARNING("Could not find runtime class descriptor for: '%s'", pClassName);
	
	return nullptr;
}

u32 findRuntimeDerivedClasses(const char* pBaseClassName, Array<IRuntimeClass*>& rClasses, bool bTagged, u32 tag)
{
	bool bAdd;

	for (size_t i = 0, iCount = s_runtimeClasses.size(); i < iCount; ++i)
	{
		if (!strcmp(s_runtimeClasses[i]->baseClassName(), pBaseClassName))
		{
			bAdd = true;

			if (bTagged)
			{
				if (s_runtimeClasses[i]->tag() != tag)
				{
					bAdd = false;
				}
			}

			if (bAdd)
			{
				rClasses.append(s_runtimeClasses[i]);
			}
		}
	}

	return rClasses.size();
}

IRuntimeClass* findFirstRuntimeDerivedClass(const char* pBaseClassName)
{
	for (size_t i = 0, iCount = s_runtimeClasses.size(); i < iCount; ++i)
	{
		if (!strcmp(s_runtimeClasses[i]->baseClassName(), pBaseClassName))
		{
			B_LOG_INFO("Found first class '%s' derived from '%s'",
				s_runtimeClasses[i]->className(), pBaseClassName);

			return s_runtimeClasses[i];
		}
	}

	B_LOG_WARNING("Could not find a first runtime class descriptor for base class: '%s'", pBaseClassName);
	
	return nullptr;
}

}