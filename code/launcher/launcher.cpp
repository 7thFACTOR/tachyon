// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include "base/cmdline_arguments.h"
#include "core/application.h"
#include "base/process.h"
#include "base/task_scheduler.h"
#include "base/math/util.h"
#include "core/globals.h"
#include "core/module.h"
#include "base/variable.h"
#include "base/string.h"
#include "base/logger.h"
#include "core/resource_repository.h"
#include "logic/logic.h"
#include "graphics/graphics.h"
#include "input/window.h"
#include "base/stdio_logger.h"
#include "core/resource.h"
#include "base/math/conversion.h"
#include "graphics/shape_renderer.h"
#include "base/file_change_monitor.h"

#include "logic/component.h"
#include "logic/entity.h"
#include "logic/logic.h"

#ifdef _WINDOWS
	#include <wchar.h>
	#include <tchar.h>
	#include <windows.h>
	#include "resource.h"
#endif

#ifdef _LINUX
	#include <unistd.h>
#endif

using namespace base;
using namespace engine;

String configPath = "../config/";
String dataPath = "../assets/engine/.builds/win64dev";
String modulePath = "./modules/";
String bundleAddonsPath = "../data/addons";
FileChangeMonitor mon;

class LauncherApplication : public Application
{
public:

	LauncherApplication()
	{
		setApplication(this);
	}

	~LauncherApplication()
	{}
	
	void initialize()
	{
		getLogic().inputMap.load(configPath + "app.input");
		auto mainWorld = getLogic().createWorld();
		getLogic().setActiveWorld(mainWorld);
		getGraphics().getWindow()->setMouseCursorVisible(false);
		getGraphics().getWindow()->setActive();
	}
	
	void update() override
	{
		Application::update();
		mon.update();
		getLogic().update();
		getResources().update();
		getGraphics().update();
		getTaskScheduler().reset();
		inputQueue.clearEvents();
	}
};

#if !defined(_CONSOLE) && defined(_WINDOWS)
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
#elif defined(_CONSOLE) && defined(_WINDOWS)
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	CommandLineArguments args;

#if defined(_WINDOWS)
	args.parse(__argc, __wargv);
#else
	args.parse(argc, argv);
#endif
	StdioLogger stdioLog;

	getBaseLogger().linkChild(&stdioLog);

	B_LOG_INFO("UTF8 " << args.getArgValue("huan"));

	// lets compile any assets that were changed lately
#ifndef _SHIPPING
	Process acEngine("asset_compiler.exe", "../assets/engine", ProcessFlags::WaitToFinish);
#endif

	B_LOG_INFO("Creating engine globals...");
	createEngineGlobals();

	getVariableRegistry().registerVariable("data_path", &dataPath, VariantType::String, dataPath);

	B_LOG_INFO("Loading engine config...");
	getVariableRegistry().loadVariables(configPath + "engine.config");
	B_LOG_INFO("Loading app config...");
	getVariableRegistry().loadVariables(args.getArgValue("config", configPath + "app.config"));
	B_LOG_INFO("Loading command args config...");
	getVariableRegistry().loadVariables(args);

	getVariableRegistry().setVariableValue<i32>("sys_app_icon_id", IDI_LAUNCHER);

	String buildTypeName = "";

#if defined(_EDITOR) && defined(_DEBUG)
	getVariableRegistry().setVariableValue<String>("sys_build_type", "debug_editor");
	buildTypeName = "[DebugEditor]";
#elif defined(_EDITOR) && defined(_DEVELOPMENT)
	getVariableRegistry().setVariableValue<String>("sys_build_type", "development_editor");
	buildTypeName = "[DevelopmentEditor]";
#elif defined(_EDITOR) && defined(_SHIPPING)
	getVariableRegistry().setVariableValue<String>("sys_build_type", "shipping_editor");
	buildTypeName = "[ShippingEditor]";
#elif _DEBUG
	getVariableRegistry().setVariableValue<String>("sys_build_type", "debug");
	buildTypeName = "[Debug]";
#elif _DEVELOPMENT
	getVariableRegistry().setVariableValue<String>("sys_build_type", "development");
	buildTypeName = "[Development]";
#elif _SHIPPING
	getVariableRegistry().setVariableValue<String>("sys_build_type", "shipping");
#endif

	B_LOG_INFO("Loading data bundles...");

	getResources().addBundlesFrom(getVariableRegistry().getVariableValue<String>("data_path"), true);
	getResources().addBundlesFrom(bundleAddonsPath, true);
	getVariableRegistry().setVariableValue<String>("sys_app_title", "Tachyon Launcher " + buildTypeName);

	LauncherApplication app;

#ifndef _SHIPPING
	struct MyFileChangeObserver : FileChangeObserver
	{
		void onFileChangeNotification(const FileChangeNotification& n) override
		{
			String action;

			switch (n.changeType)
			{
			case FileChangeNotification::FileCreated: action = "created"; break;
			case FileChangeNotification::FileModified: action = "modified"; break;
			case FileChangeNotification::FileNewName: action = "new name"; break;
			case FileChangeNotification::FileOldName: action = "old name"; break;
			case FileChangeNotification::FileDeleted: action = "deleted"; break;
			default:
				break;
			}

			B_LOG_INFO("File notification: " << n.filename << " " << action);

			String s;
			loadTextFile("../assets/" + n.filename, s);

			B_LOG_INFO(s);
		}
	} fcn;

	mon.addObserver(&fcn);
	mon.watchPath("../assets", true);
#endif

	initializeEngine();

	app.initialize();
	
	// preload
	loadResource("gpu_programs/default.gpu_program");
	loadResource("materials/default.material");
	loadResource("textures/default.psd");

	getModuleLoader().onLoadModules();
	getModuleLoader().onBeginSimulateModules();
	app.run();
	getModuleLoader().onEndSimulateModules();
	getModuleLoader().onUnloadModules();
	shutdownEngine();

	return 0;
}