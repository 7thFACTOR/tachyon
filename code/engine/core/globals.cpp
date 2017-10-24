// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/globals.h"
#include "base/assert.h"
#include "base/logger.h"
#include "base/html_logger.h"
#include "base/version.h"
#include "base/util.h"
#include "core/application.h"
#include "base/json.h"
#include "logic/logic.h"
#include "audio/audio.h"
#include "physics/physics.h"
#include "core/resource_repository.h"
#include "base/profiler.h"
#include "base/statistics.h"
#include "base/assert.h"
#include "input/window.h"
#include "base/timer.h"
#include "core/module.h"
#include "input/window.h"
#include "base/variable.h"
#include "graphics/graphics.h"
#include "base/task_scheduler.h"
#include "base/variable.h"
#include "base/parameter_list.h"
#include "base/code_coverage.h"

#ifdef _WINDOWS
#include <windows.h>
#endif
#include "graphics/opengl/opengl_graphics.h"

namespace engine
{
using namespace base;
class Globals;

Globals* globals = nullptr;

class Globals
{
public:
	Globals()
	{
		engineMode = EngineMode::Playing;
		engineHost = EngineHost::Launcher;
		window = nullptr;
		language = "en";
		B_LOG_INFO("Init resource repository...");
		resourceRepository.initialize();
		B_LOG_INFO("Init task scheduler...");
		taskScheduler.initialize();

		B_LOG_INFO("Registering engine variables...");
		variables.registerVariable("sys_create_html_log", &sys_create_html_log, VariantType::Bool, false, "if true, create a HTML log");
		variables.registerVariable("sys_app_icon_id", &sys_app_icon_id, VariantType::Int32, 1, "the render window icon id from resource file");
		variables.registerVariable("sys_app_title", &sys_app_title, VariantType::String, "Engine", "the title of the app's main window");
		variables.registerVariable("sys_edit_mode", &sys_edit_mode, VariantType::Bool, false, "set if the engine is in editing mode, summoned by tools/editor");
		variables.registerVariable("sys_modules_path", &sys_modules_path, VariantType::String, "./modules/", "the modules path");
		variables.registerVariable("sys_build_type", &sys_build_type, VariantType::String, "development", "current build type, development or release", VariableFlags::ReadOnly);
		variables.registerVariable("sys_engine_bundle_toc", &sys_engine_bundle_toc, VariantType::String, "../data/engine.toc", "");

		variables.registerVariable("gfx_device_index", &gfx_device_index, VariantType::Int32, 0, "the video device index");
		variables.registerVariable("gfx_screen_width", &gfx_screen_width, VariantType::Int32, 1024, "video resolution width", VariableFlags::ReadOnly);
		variables.registerVariable("gfx_screen_height", &gfx_screen_height, VariantType::Int32, 768, "video resolution height", VariableFlags::ReadOnly);
		variables.registerVariable("gfx_refresh_rate", &gfx_refresh_rate, VariantType::Int32, 0, "screen refresh rate");
		variables.registerVariable("gfx_vsync", &gfx_vsync, VariantType::Bool, true, "is vsync on");
		variables.registerVariable("gfx_antialiasing", &gfx_antialiasing, VariantType::Int32, 0, "FSAA mode");
		variables.registerVariable("gfx_fullscreen", &gfx_fullscreen, VariantType::Bool, false, "is video fullscreen");
		variables.registerVariable("gfx_api", &gfx_api, VariantType::String, "opengl", "video api used: opengl, vulkan, d3d, metal");

		variables.registerVariable("snd_output", &snd_output, VariantType::Int32, 0);
		variables.registerVariable("snd_driver", &snd_driver, VariantType::Int32, 0);
		variables.registerVariable("snd_frequency", &snd_frequency, VariantType::Int32, 0);

		variables.registerVariable("inp_wrap_mouse", &inp_wrap_mouse, VariantType::Bool, false);
		variables.registerVariable("inp_mouse_speed", &inp_mouse_speed, VariantType::Float, 50);
		B_LOG_INFO("End globals creation...");
	}

	~Globals()
	{
		globals = nullptr;
	}

	// core variables
	bool sys_create_html_log;
	i32 sys_app_icon_id;
	String sys_app_title;
	bool sys_edit_mode;
	String sys_modules_path;
	String sys_build_type;
	String sys_engine_bundle_toc;
		
	String gfx_api;
	i32 gfx_device_index;
	i32 gfx_screen_width;
	i32 gfx_screen_height;
	i32 gfx_refresh_rate;
	bool gfx_vsync;
	i32 gfx_antialiasing;
	bool gfx_fullscreen;

	i32 snd_output;
	i32 snd_driver;
	i32 snd_frequency;
	
	bool inp_wrap_mouse = true;
	f32 inp_mouse_speed = 50;
	
	String language = "en";
	Application* application = nullptr;
	Graphics* graphics = nullptr;

	Audio audio;
	Physics physics;
	Logic logic;
	ClassRegistry classRegistry;
	CodeCoverage codeCoverage;
	VariableRegistry variables;
	TaskScheduler taskScheduler;
	ResourceRepository resourceRepository;
	ModuleLoader moduleLoader;
	Profiler profiler;
	Statistics statistics;
	Clock clock;
	EngineMode engineMode = EngineMode::Playing;
	EngineHost engineHost = EngineHost::Launcher;
	Window* window = nullptr;

};

inline Globals* createGlobals()
{
	if (!globals)
	{
		globals = new Globals();
	}

	return globals;
}

//---

Graphics& getGraphics()
{
	B_ASSERT(globals);
	B_ASSERT(globals->graphics);
	return *globals->graphics;
}

Audio& getAudio()
{
	B_ASSERT(globals);
	return globals->audio;
}

Logic& getLogic()
{
	B_ASSERT(globals);
	return globals->logic;
}

Physics& getPhysics()
{
	B_ASSERT(globals);
	return globals->physics;
}

TaskScheduler& getTaskScheduler()
{
	B_ASSERT(globals);
	return globals->taskScheduler;
}

Clock& getClock()
{
	B_ASSERT(globals);
	return globals->clock;
}

ModuleLoader& getModuleLoader()
{
	B_ASSERT(globals);
	return globals->moduleLoader;
}

EngineMode getEngineMode()
{
	B_ASSERT(globals);
	return globals->engineMode;
}

EngineHost getEngineHost()
{
	B_ASSERT(globals);
	return globals->engineHost;
}

ResourceRepository& getResources()
{
	if (!globals)
	{
		createGlobals();
	}

	B_ASSERT(globals);
	return globals->resourceRepository;
}

void setApplication(Application* app)
{
	B_ASSERT(globals);
	globals->application = app;
}

Application& getApplication()
{
	B_ASSERT(globals);
	B_ASSERT(globals->application);
	return *globals->application;
}

void setEngineMode(EngineMode mode)
{
	B_ASSERT(globals);
	globals->engineMode = mode;
}

void setEngineHost(EngineHost host)
{
	B_ASSERT(globals);
	globals->engineHost = host;
}

bool loadModules(const String& path)
{
	B_ASSERT(globals);
	globals->moduleLoader.loadModules(path, moduleFilenameMask);

	return globals->moduleLoader.getModules().size() != 0;
}

bool freeModules()
{
	B_ASSERT(globals);
	globals->moduleLoader.unloadModules();
	return true;
}

bool createEngineGlobals()
{
	return createGlobals() != nullptr;
}

void destroyEngineGlobals()
{
	B_SAFE_DELETE(globals);
}

bool initializeEngine()
{
	SystemInfo sysInfo;

	createGlobals();
	globals->graphics = nullptr;

	//TODO: make it more clear
	getClock().createTimer();// logic
	getClock().createTimer();// gui
	getClock().createTimer();// user

	if (!base::getBaseLogger().isOpened())
	{
		base::getBaseLogger().open(getApplicationFilename() + ".log");
	}

	B_LOG_INFO("************************************");
	B_LOG_INFO("*** Starting engine initialization sequence...");

	sysInfo = getSystemInfo();

	B_LOG_INFO("*** System Info ***");

	{
		B_LOG_AUTO_INDENT;
		B_LOG_INFO("Operating system: " << sysInfo.operatingSystemInfo);
		B_LOG_INFO("System memory: " << convertBytesToText(sysInfo.totalRAM) << " total, " << convertBytesToText(sysInfo.freeRAM) << " free");
		B_LOG_INFO("Page size: " << convertBytesToText(sysInfo.pageSize));
		B_LOG_INFO("Processor architecture: " << (sysInfo.processorArchitecture == ProcessorArchitecture::Cpu32bit
			? "32bit" : "64bit"));
		B_LOG_INFO("Processor count: " << sysInfo.physicalProcessorCount);
		B_LOG_INFO("Cores count: " << sysInfo.processorCoreCount);
		B_LOG_INFO("Logical processor count: " << sysInfo.logicalProcessorCount);
		B_LOG_INFO("Processor cache count: L1: " << sysInfo.level1CacheCount << " L2: " << sysInfo.level2CacheCount << " L3: " << sysInfo.level3CacheCount);
	}

	bool isEditMode = globals->sys_edit_mode;

	if (isEditMode)
	{
		setEngineHost(EngineHost::Editor);
		setEngineMode(EngineMode::Editing);
	}

#ifndef E_STATIC
	// load the modules only if not static build, since that will link everything and all modules in one big exe
	loadModules(globals->sys_modules_path);
#endif

	B_LOG_INFO("Register modules runtime classes...");
	getModuleLoader().registerModuleRuntimeClasses();

	B_LOG_INFO("*** Core objects setup ***");

	//////////////////////////////////////////////////////////////////////////
	// Graphics init
	//////////////////////////////////////////////////////////////////////////

	if (globals->gfx_api == "opengl")
	{
		globals->graphics = new OpenglGraphics();
	}
	else if (globals->gfx_api == "vulkan")
	{
		//globals->graphics = new VulkanGraphics();
	}
	else if (globals->gfx_api == "direct3d")
	{
		//globals->graphics = new Direct3DGraphics();
	}
	else if (globals->gfx_api == "metal")
	{
		//globals->graphics = new MetalGraphics();
	}
	else
	{
		B_LOG_ERROR("No graphics api specified in the gfx_api variable, please set one");
		return false;
	}

	u32 videoAdapterIndex = globals->gfx_device_index;
	u32 width, height, bpp, refreshRate;
	bool fullScreen;

	width = globals->gfx_screen_width;
	height = globals->gfx_screen_height;
	bpp = 32;
	refreshRate = globals->gfx_refresh_rate;
	fullScreen = globals->gfx_fullscreen;

	VideoMode newVideoMode;
	VideoMode* bestVideoMode = nullptr;
	Window* wnd = new Window({ width, height }, globals->sys_app_title);
	B_ASSERT(wnd);

	globals->window = wnd;

	// init graphics
	globals->graphics->setWindow(globals->window);

	B_LOG_INFO("Awakening graphics...");
	B_LOG_INFO("VerticalSync: " << (globals->gfx_vsync ? "enabled" : "disabled"));

	if (!globals->graphics->getWindow())
	{
		B_LOG_WARNING("There is no render window set in the graphics api, no rendering will occur.");
	}
	else
	{
		globals->graphics->getWindow()->requestFocus();
	}

	globals->graphics->initialize();

	// check video mode if available
	if (globals->graphics->getVideoAdapters().size())
	{
		// find and set the requested video mode
		bestVideoMode = globals->graphics->getVideoAdapters()[videoAdapterIndex].findVideoMode(width, height, bpp, refreshRate);

		if (!bestVideoMode)
		{
			B_LOG_WARNING("Video mode [ " << width << " x " << height << " x " << bpp << " bpp ] was not found in the device's video modes list. Fallback to setup settings");
			newVideoMode.adapterIndex = videoAdapterIndex;
			newVideoMode.width = globals->gfx_screen_width;
			newVideoMode.height = globals->gfx_screen_height;
			newVideoMode.fullScreen = fullScreen;
			newVideoMode.bitsPerPixel = 32;
			newVideoMode.frequency = globals->gfx_refresh_rate;
		}
		else
		{
			newVideoMode = *bestVideoMode;
		}

		newVideoMode.fullScreen = fullScreen;

		B_LOG_INFO("Current video settings: Adapter Index: " << videoAdapterIndex << " Screen Size: " << newVideoMode.width << "x" << newVideoMode.height << " Fullscreen: " << newVideoMode.fullScreen);

		// resize window for the chosen video mode
		if (globals->graphics->getWindow())
		{
			if (!globals->graphics->getVideoAdapters().isEmpty())
			{
				globals->graphics->getWindow()->setSize(
					Vec2u(newVideoMode.width, newVideoMode.height));
			}
		}

		B_LOG_INFO("Applying video mode...");
		globals->graphics->changeVideoMode(newVideoMode);
	}

	B_LOG_INFO("Initialize audio...");
	globals->audio.initialize();
	B_LOG_INFO("Initialize network...");
	//globals->network.initialize();
	B_LOG_INFO("Initialize physics...");
	globals->physics.initialize();
	B_LOG_INFO("Initialize logic...");
	globals->logic.initialize();

	B_LOG_SUCCESS("Engine setup done.");
	B_LOG_INFO("*** Engine initialized ***");

	return true;
}

static void writePerfProfilerSection(ProfilerSection* section, u32 level = 0)
{
	B_ASSERT(globals);
	String indent, percent;

	indent.repeat(" ", level * 4);

	if (section->parent)
	{
		percent << String::floatDecimals(2) << section->percentTimeFromParent << "%";
	}

	B_LOG_INFO(indent << percent << "[#" << section->name << "] - Calls: " << section->executeCount << ", Total: " << section->totalTime << "ms, Peak: " << section->peakTime << "ms, Avg: " << section->averageTime << "ms");

	if (!section->children.isEmpty())
	{
		B_LOG_INFO(indent << "[");
		++level;

		for (auto childSection : section->children)
		{
			writePerfProfilerSection(childSection, level);
		}

		B_LOG_INFO(indent << "]");
	}
}

void shutdownEngine()
{
	B_ASSERT(globals);
	B_LOG_INFO("*** Started shutdown sequence ***");

	B_LOG_INFO("Unloading modules...");
	freeModules();

	globals->logic.shutdown();

	B_LOG_INFO("Unloading resources...");
	globals->resourceRepository.unloadAll();
	B_LOG_INFO("Shutdown graphics...");

	if (globals->graphics)
	{
		globals->graphics->shutdown();
		B_SAFE_DELETE(globals->graphics);
	}

	B_LOG_INFO("Shutdown audio...");
	globals->audio.shutdown();
	B_LOG_INFO("Shutdown network...");
	//globals->network.shutdown();
	B_LOG_INFO("Shutdown physics...");
	globals->physics.shutdown();

	// if the main rendering window was created by the engine, then destroy it
	B_LOG_INFO("Destroying window...");
	B_SAFE_DELETE(globals->window);

	// spit out profiler info
	B_LOG_INFO("*** Performance profiler report ***");
	B_LOG_INFO("Profiler sections:");
	getProfiler().computeSectionsPercentTime();

	for (auto& section : getProfiler().getSections())
	{
		if (!section.value->parent)
		{
			writePerfProfilerSection(section.value);
		}
	}

	B_LOG_INFO("Shutdown task scheduler...");
	globals->taskScheduler.shutdown();

	B_LOG_INFO("*** Shutdown terminated ***");
	B_LOG_INFO("*** Exit ***");
	B_SAFE_DELETE(globals);
	deleteMainAllocator();
	// FIN! //
}

Profiler& getProfiler()
{
	B_ASSERT(globals);
	
	return globals->profiler;
}

ClassRegistry& getClassRegistry()
{
	createGlobals();
	B_ASSERT(globals);
	return globals->classRegistry;
}

VariableRegistry& getVariableRegistry()
{
	createGlobals();
	B_ASSERT(globals);
	return globals->variables;
}

Statistics& getStatistics()
{
	B_ASSERT(globals);
	
	return globals->statistics;
}

String getEngineVersionString()
{
	B_ASSERT(globals);
	Version ver;

	//TODO: autogenerate revision_info.h and include here
	ver.majorVersion = 1;
	ver.minorVersion = 0;
	ver.revision = 0;
	
	return
		String("v") << ver.majorVersion << "."
		<< ver.minorVersion << "."
		<< ver.revision;
}

void setCurrentLanguage(const String& twoLetterLangName)
{
	if (!globals)
	{
		createGlobals();
	}

	globals->language = twoLetterLangName;
}

const String& getCurrentLanguage()
{
	if (!globals)
	{
		createGlobals();
	}

	return globals->language;
}

}