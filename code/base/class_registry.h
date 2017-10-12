// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/types.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/array.h"

namespace base
{
class ClassDescriptor;

class RuntimeClass
{
public:
	virtual bool copyFrom(RuntimeClass* from) { return false; }
	virtual ClassDescriptor* getClassDescriptor() const = 0;
};

class B_API ClassDescriptor
{
public:
	virtual ~ClassDescriptor(){}
	virtual RuntimeClass* newInstance() = 0;
	virtual String getClassName() const = 0;
	virtual String getBaseClassName() const = 0;
	inline bool is(const String& className) const { return getClassName() == className; }
	inline bool derivedFrom(const String& baseClassName) const { return getBaseClassName() == baseClassName; }
};

class B_API ClassRegistry
{
public:
	RuntimeClass* newClassInstance(const String& className);
	void registerClass(ClassDescriptor* classDesc);
	void unregisterClass(ClassDescriptor* classDesc);
	ClassDescriptor* findClass(const String& className);
	size_t findDerivedClasses(const String& baseClassName, Array<ClassDescriptor*>& outClasses);
	ClassDescriptor* findFirstDerivedClass(const String& baseClassName);

protected:
	Array<ClassDescriptor*> classes;
};

#define B_DECLARE_CLASS(className)\
	static class className##ThisClassDescriptor : public base::ClassDescriptor\
	{\
	public:\
		RuntimeClass* newInstance() override { return new className(); }\
		String getClassName() const override { return ""#className; }\
		String getBaseClassName() const override { return ""; }\
	} className##ThisClassDescriptorMember;\
	base::ClassDescriptor* getClassDescriptor() const override { return &className##ThisClassDescriptorMember; }\
	static base::ClassDescriptor* getStaticClassDescriptor() { return &className##ThisClassDescriptorMember; }

#define B_DECLARE_DERIVED_CLASS(className, baseClassName)\
	static class className##ThisClassDescriptor : public base::ClassDescriptor\
	{\
	public:\
		RuntimeClass* newInstance() override { return new className(); }\
		String getClassName() const override { return ""#className; }\
		String getBaseClassName() const override { return ""#baseClassName; }\
	} className##ThisClassDescriptorMember; \
	base::ClassDescriptor* getClassDescriptor() const override { return &className##ThisClassDescriptorMember; }\
	static base::ClassDescriptor* getStaticClassDescriptor() { return &className##ThisClassDescriptorMember; }

#define B_DEFINE_CLASS(className) className::className##ThisClassDescriptor className::className##ThisClassDescriptorMember;

#define B_REGISTER_RUNTIME_CLASS(classRegistry, className)\
	classRegistry.registerClass(className::getStaticClassDescriptor());
}