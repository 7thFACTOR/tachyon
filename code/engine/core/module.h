#pragma once
#include "base/string.h"
#include "base/dictionary.h"
#include "base/defines.h"
#include "core/defines.h"
#include "base/types.h"
#include "base/dynamic_library.h"
#include "base/version.h"

namespace engine
{
using namespace base;

class ModuleDescriptor;
class Module;

#ifdef _WINDOWS
const String moduleFilenameMask = "*.dll";
#else
const String moduleFilenameMask = "*.so";
#endif

typedef ModuleDescriptor* (*ModuleDescriptorFunction)();

class E_API ModuleDescriptor
{
public:
	virtual const char* getName() const = 0;
	virtual const char* getDescription() const = 0;
	virtual const char* getCopyright() const = 0;
	virtual Version getVersion() const = 0;
	virtual Module* createInstance() const = 0;
};

struct ModuleInfo
{
	DynamicLibrary library;
	ModuleDescriptor* descriptor = nullptr;
};

//! This class is inherited by the user to create a new reloadable C++ module
class E_API Module
{
public:
	Module() {}
	virtual ~Module() {}
	virtual void onRegisterRuntimeClasses() {}
	virtual void onUnregisterRuntimeClasses() {}
	virtual void onLoad() {}
	virtual void onUnload() {}
	virtual void onBeginSimulation() {}
	virtual void onEndSimulation() {}
	virtual void onUpdate(f32 deltaTime) {}
	virtual bool isReloadable() const { return true; }
};

class E_API ModuleLoader
{
public:
	ModuleLoader();
	//! creates the object and loads all modules from path using the file mask, ex.: "*.module"
	ModuleLoader(const String& path, const String& filenameMask = moduleFilenameMask);
	virtual ~ModuleLoader();

	//! \return the number of loaded modules
	void loadModules(const String& path, const String& filenameMask = moduleFilenameMask);
	bool loadModule(const String& moduleFilename);
	bool unloadModule(const String& moduleFilename);
	bool addModule(ModuleDescriptor* desc);
	bool removeModule(ModuleDescriptor* desc);
	void unloadModules();
	inline const Dictionary<ModuleInfo*, Module*>& getModules() const { return modules; }
	void registerModuleRuntimeClasses();
	void unregisterModuleRuntimeClasses();
	void onLoadModules();
	void onUnloadModules();
	void onBeginSimulateModules();
	void onEndSimulateModules();

private:
	Dictionary<ModuleInfo*, Module*> modules;
};

// Handy macros for speeding up
// the module creation process
#if defined(E_STATIC)
#define E_DECLARE_MODULE(moduleClassName, moduleDescription, moduleCopyright, moduleVersion)\
		class moduleClassName##ModuleDescriptor : public engine::ModuleDescriptor\
		{public:\
			const char* getName() const override { return ""#moduleClassName; }\
			const char* getDescription() const override { return moduleDescription; }\
			const char* getCopyright() const override { return moduleCopyright; }\
			base::Version getVersion() const override { return moduleVersion; }\
			engine::Module* createInstance() const override { return new moduleClassName(); }\
		} moduleDescriptor##moduleClassName;
#else
	//! define a module descriptor, which describes a module
#define E_DECLARE_MODULE(moduleClassName, moduleDescription, moduleCopyright, moduleVersion)\
		class moduleClassName##ModuleDescriptor : public engine::ModuleDescriptor\
		{public:\
			const char* getName() const override { return ""#moduleClassName; }\
			const char* getDescription() const override { return moduleDescription; }\
			const char* getCopyright() const override { return moduleCopyright; }\
			base::Version getVersion() const override { return moduleVersion; }\
			engine::Module* createInstance() const override { return new moduleClassName(); }\
		} moduleDescriptor##moduleClassName;\
		B_CAPI_EXPORT engine::ModuleDescriptor* engineGetModuleDescriptor() { return (engine::ModuleDescriptor*)&moduleDescriptor##moduleClassName; };
#endif

}