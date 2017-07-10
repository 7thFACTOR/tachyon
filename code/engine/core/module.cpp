#include "core/module.h"
#include "base/platform.h"
#include "base/util.h"
#include "core/globals.h"
#include "base/logger.h"
#include "base/assert.h"

namespace engine
{
ModuleLoader::ModuleLoader()
{}

ModuleLoader::ModuleLoader(const String& path, const String& filenameMask)
{
	loadModules(path, filenameMask);
}

ModuleLoader::~ModuleLoader()
{
	unloadModules();
}

void ModuleLoader::loadModules(const String& path, const String& filenameMask)
{
	String fullname;
	FindFileHandle hFind;
	FoundFileInfo ffi;

	B_LOG_INFO("Searching modules inside folder '" << path << "' and mask '" << filenameMask << "'");

	if (!(hFind = findFirstFile(path, filenameMask, ffi)))
	{
		B_LOG_INFO("No modules found in '" << path << "' with mask '" << filenameMask << "'");
		return;
	}

	do
	{
		B_LOG_INFO("Checking module: " << ffi.filename);
		fullname = mergePathName(path, ffi.filename);
		loadModule(fullname);

	} while (findNextFile(hFind, ffi));
}

bool ModuleLoader::loadModule(const String& filename)
{
	bool duplicate = false;

	for (auto module : modules)
	{
		if (module.key->library.getFilename() == filename)
		{
			B_LOG_WARNING("Library '" << filename << "' already loaded, ignoring...");
			duplicate = true;
			break;
		}
	}

	if (duplicate)
		return false;

	ModuleInfo* moduleInfo = new ModuleInfo();
	Module* module = nullptr;

	if (!moduleInfo->library.load(filename))
	{
		B_LOG_ERROR("Could not load module " << filename);
		return false;
	}

	ModuleDescriptorFunction func = (ModuleDescriptorFunction)moduleInfo->library.findFunctionAddress("engineGetModuleDescriptor");

	if (!func)
	{
		B_LOG_ERROR("Could not find engineGetModuleDescriptor entry point function in module " << filename);
		return false;
	}

	moduleInfo->descriptor = func();
	module = moduleInfo->descriptor->createInstance();

	B_LOG_SUCCESS("Loaded module: '" << moduleInfo->descriptor->getName() << "' (from file: " << filename << ")");
	modules.add(moduleInfo, module);

	return true;
}

bool ModuleLoader::unloadModule(const String& moduleFilename)
{
	for (auto module : modules)
	{
		if (module.key->library.getFilename() == moduleFilename)
		{
			modules.erase(module.key);
			delete module.value;
			// will also unload the library
			delete module.key;

			return true;
		}
	}

	return false;
}

bool ModuleLoader::addModule(ModuleDescriptor* desc)
{
	ModuleInfo* moduleInfo = new ModuleInfo();
	Module* module = nullptr;

	B_ASSERT(moduleInfo);

	moduleInfo->descriptor = desc;
	module = moduleInfo->descriptor->createInstance();

	B_ASSERT(module);

	if (!module)
	{
		return false;
	}

	B_LOG_SUCCESS("Added module: '" << moduleInfo->descriptor->getName() << "'");
	modules.add(moduleInfo, module);

	return true;
}

bool ModuleLoader::removeModule(ModuleDescriptor* desc)
{
	for (auto module : modules)
	{
		if (module.key->descriptor == desc)
		{
			modules.erase(module.key);
			delete module.value;
			// will also unload the library
			delete module.key;

			return true;
		}
	}

	return false;
}

void ModuleLoader::unloadModules()
{
	for (auto& module : modules)
	{
		delete module.value;
		// will also unload the library
		delete module.key;
	}

	modules.clear();
}

void ModuleLoader::registerModuleRuntimeClasses()
{
	for (auto& module : modules)
	{
		module.value->onRegisterRuntimeClasses();
	}
}

void ModuleLoader::unregisterModuleRuntimeClasses()
{
	for (auto& module : modules)
	{
		module.value->onUnregisterRuntimeClasses();
	}
}

void ModuleLoader::onLoadModules()
{
	for (auto& module : modules)
	{
		module.value->onLoad();
	}
}

void ModuleLoader::onUnloadModules()
{
	for (auto& module : modules)
	{
		module.value->onUnload();
	}
}

void ModuleLoader::onBeginSimulateModules()
{
	for (auto& module : modules)
	{
		module.value->onBeginSimulation();
	}
}

void ModuleLoader::onEndSimulateModules()
{
	for (auto& module : modules)
	{
		module.value->onEndSimulation();
	}
}

}