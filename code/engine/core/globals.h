// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/string.h"
#include "base/array.h"
#include "base/dictionary.h"
#include "base/memory.h"
#include "base/types.h"
#include "base/variant.h"
#include "core/defines.h"
#include "core/types.h"
#include "base/observer.h"

namespace base
{
class Profiler;
class Clock;
class Statistics;
class TaskScheduler;
class ClassRegistry;
class VariableRegistry;
}

namespace engine
{
using namespace base;

class Application;
class Audio;
class Physics;
class Graphics;
class Logic;
class ResourceRepository;
class ModuleLoader;

E_EXPORT bool createEngineGlobals();
E_EXPORT bool initializeEngine();
E_EXPORT void shutdownEngine();
E_EXPORT void destroyEngineGlobals();
E_EXPORT EngineMode getEngineMode();
E_EXPORT EngineHost getEngineHost();
E_EXPORT void setEngineMode(EngineMode mode);
E_EXPORT void setEngineHost(EngineHost host);

E_EXPORT void setApplication(Application* app);
E_EXPORT Application& getApplication();
E_EXPORT Graphics& getGraphics();
E_EXPORT Audio& getAudio();
E_EXPORT Logic& getLogic();
E_EXPORT Physics& getPhysics();
E_EXPORT Statistics& getStatistics();
E_EXPORT TaskScheduler& getTaskScheduler();
E_EXPORT ResourceRepository& getResources();
E_EXPORT ModuleLoader& getModuleLoader();
E_EXPORT Clock& getClock();
E_EXPORT Profiler& getProfiler();
E_EXPORT ClassRegistry& getClassRegistry();
E_EXPORT VariableRegistry& getVariableRegistry();

E_EXPORT bool loadModules(const String& path);
E_EXPORT bool freeModules();

E_EXPORT void setCurrentLanguage(const String& twoLetterLangName);
E_EXPORT const String& getCurrentLanguage();

E_EXPORT String getEngineVersionString();

}