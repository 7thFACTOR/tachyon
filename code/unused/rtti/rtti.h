#pragma once
#include "base/types.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/array.h"

namespace base
{
struct IRuntimeClass;
typedef u32 ClassUid;

class B_API Rtti
{
public:
	//! copy all object data from another rtti object
	virtual bool copyFrom(Rtti* pFrom);
	virtual IRuntimeClass* runtimeClass() const = 0;
	ClassUid uid() const
	{
		return m_id;
	}
	void setUid(ClassUid aId)
	{
		m_id = aId;
	}

protected:
	ClassUid m_id;
};

class B_API Naming
{
public:
	const String& name() const
	{
		return m_name;
	}

	void setName(const char* pName)
	{
		m_name = pName;
	}

protected:
	String m_name;
};

//! This holds the info about a runtime C++ class from inside a plugin or engine
struct B_API IRuntimeClass
{
	virtual ~IRuntimeClass(){}
	//! \return am instance of the class
	virtual void* newInstance() = 0;
	//! \return the class name
	virtual const char* className() const = 0;
	virtual const char* baseClassName() const = 0;
	virtual u32 tag() const { return 0; }
	bool is(const char* pClassName);
	bool derivedFrom(const char* pBaseClassName);
};

//! create a new class instance based on class name, from the plugins or registered class factory callbacks
B_EXPORT Rtti* newClassInstance(const char* pClassName);
//! registers a class factory callback, used by the newClassInstance() if it does not find the specified class name in the plugins, then
//! it will call these callbacks, until one of them returns a not nullptr Object class pointer, with the newly created object instance\n
//! In the callback you must check if you support that class, create an instance and return that pointer, else return nullptr
B_EXPORT void registerRuntimeClass(IRuntimeClass* pClassDesc);
//! unregister a class factory callback
B_EXPORT void unregisterRuntimeClass(IRuntimeClass* pClassDesc);
//! \return the class descriptor by class name, if any
B_EXPORT IRuntimeClass* findRuntimeClass(const char* pClassName);
//! return the class descriptors by base class name, if any
B_EXPORT u32 findRuntimeDerivedClasses(const char* pBaseClassName, Array<IRuntimeClass*>& rClasses, bool bTagged = false, u32 tag = 0);
//! return the first derived class descriptor by base class name, if any
B_EXPORT IRuntimeClass* findFirstRuntimeDerivedClass(const char* pBaseClassName);

//! to be used in the class declaration so it will override the runtimeClass method; used with B_REGISTER_CLASS
#define B_RUNTIME_CLASS \
	virtual IRuntimeClass* runtimeClass() const;\
	static IRuntimeClass* staticRuntimeClass();

//! define a class descriptor, which describes a runtime class characteristics
#define B_REGISTER_CLASS(theClassName)\
	struct RuntimeClassDescriptor##theClassName : IRuntimeClass\
	{\
		RuntimeClassDescriptor##theClassName()\
		{\
			base::registerRuntimeClass(this);\
		}\
		~RuntimeClassDescriptor##theClassName()\
		{\
			base::unregisterRuntimeClass(this);\
		}\
		void* newInstance() { return new theClassName(); }\
		const char* className() const { return ""#theClassName; }\
		const char* baseClassName() const { return ""; }\
	} s_runtimeClassDescriptor##theClassName;\
	IRuntimeClass* theClassName::runtimeClass() const { return &s_runtimeClassDescriptor##theClassName; }\
	IRuntimeClass* theClassName::staticRuntimeClass() { return &s_runtimeClassDescriptor##theClassName; }\
	int s_oneBigExeForceLinkClass##theClassName = 0;

//! define a class descriptor, which describes a abstract runtime class characteristics
#define B_REGISTER_ABSTRACT_CLASS(theClassName)\
	struct RuntimeClassDescriptor##theClassName : IRuntimeClass\
	{\
		RuntimeClassDescriptor##theClassName()\
		{\
			base::registerRuntimeClass(this);\
		}\
		~RuntimeClassDescriptor##theClassName()\
		{\
			base::unregisterRuntimeClass(this);\
		}\
		void* newInstance() { return nullptr; }\
		const char* className() const { return ""#theClassName; }\
		const char* baseClassName() const { return ""; }\
	} s_runtimeClassDescriptor##theClassName;\
	IRuntimeClass* theClassName::runtimeClass() const { return &s_runtimeClassDescriptor##theClassName; }\
	IRuntimeClass* theClassName::staticRuntimeClass() { return &s_runtimeClassDescriptor##theClassName; }\
	int s_oneBigExeForceLinkClass##theClassName = 0;

//! define a class descriptor, which describes a abstract runtime class characteristics
#define B_REGISTER_DERIVED_CLASS(theClassName, theBaseClassName)\
	struct RuntimeClassDescriptor##theClassName : IRuntimeClass\
	{\
		RuntimeClassDescriptor##theClassName()\
		{\
			base::registerRuntimeClass(this);\
		}\
		~RuntimeClassDescriptor##theClassName()\
		{\
			base::unregisterRuntimeClass(this);\
		}\
		void* newInstance() { return new theClassName(); }\
		const char* className() const { return ""#theClassName; }\
		const char* baseClassName() const { return ""#theBaseClassName; }\
	} s_runtimeClassDescriptor##theClassName;\
	IRuntimeClass* theClassName::runtimeClass() const { return &s_runtimeClassDescriptor##theClassName; }\
	IRuntimeClass* theClassName::staticRuntimeClass() { return &s_runtimeClassDescriptor##theClassName; }\
	int s_oneBigExeForceLinkClass##theClassName = 0;

//! define a class descriptor, which describes a abstract runtime class characteristics
#define B_REGISTER_DERIVED_TAGGED_CLASS(theClassName, theBaseClassName, theTag)\
	struct RuntimeClassDescriptor##theClassName : IRuntimeClass\
	{\
		RuntimeClassDescriptor##theClassName()\
		{\
			base::registerRuntimeClass(this);\
		}\
		~RuntimeClassDescriptor##theClassName()\
		{\
			base::unregisterRuntimeClass(this);\
		}\
		void* newInstance() { return new theClassName(); }\
		const char* className() const { return ""#theClassName; }\
		const char* baseClassName() const { return ""#theBaseClassName; }\
		u32 tag() const { return theTag; }\
	} s_runtimeClassDescriptor##theClassName;\
	IRuntimeClass* theClassName::runtimeClass() const { return &s_runtimeClassDescriptor##theClassName; }\
	IRuntimeClass* theClassName::staticRuntimeClass() { return &s_runtimeClassDescriptor##theClassName; }\
	int s_oneBigExeForceLinkClass##theClassName = 0;

#if defined(B_STATIC_BUILD)
	#define B_FORCE_STATIC_LINK_CLASS(theClassName) namespace base { extern int s_oneBigExeForceLinkClass##theClassName; \
		struct AutoLink##theClassName { AutoLink##theClassName() { s_oneBigExeForceLinkClass##theClassName = 1; } } s_autoLink##theClassName; }
#else
	#define B_FORCE_STATIC_LINK_CLASS(theClassName)
#endif

}