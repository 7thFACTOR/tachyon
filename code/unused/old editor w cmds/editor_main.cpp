#pragma once
#ifdef B_PLATFORM_WIN
	#include "resource.h"
	#include <tchar.h>
#endif
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include "base/args_parser.h"
#include "core/core.h"
#include "core/module.h"
#include "base/variable.h"
#include "base/string.h"
#include "base/logger.h"
#include "core/resource_manager.h"
#include "base/application.h"
#include "editor.h"
#include "input/input_manager.h"
#include "game/game_manager.h"
#include "render/render_manager.h"
#include "gui/gui_manager.h"

#ifdef B_PLATFORM_WIN
	#include <windows.h>
#endif

#ifdef B_PLATFORM_LINUX
	#include <unistd.h>
#endif

using namespace base;
using namespace engine;

class EditorApp : public Application
{
public:
	void update()
	{
		// update all
		Application::update();
		resources().update();
		input().update();
		game().update();
		render().update();
		m_editor.update();
		ui().update();

		// render all
		render().renderScene();
		m_editor.renderGizmos();
		ui().render();

		computeFrameTiming();
	}

	Editor m_editor;
};

#ifdef B_PLATFORM_WIN
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	ArgsParser args;

#ifdef B_PLATFORM_WIN
	args.parse(__argc, __argv);
#else
	args.parse(argc, argv);
#endif

	bool bEditMode = args.hasSwitch("edit-mode");

	initializeEnvironment();
	loadVariables("../config/engine.config");
	loadVariables(args.argValue("config", "../config/game.config"));
	loadVariables("../config/editor.config");

	setVariableValue<i32>("sys_app_icon_id", IDI_EDITOR);
	resources().addBundle("../data/engine.toc");
	resources().addBundle("../data/editor.toc");

	if (!initializeEngine())
	{
		return false;
	}
	
	EditorApp editorApp;

	setApplication(&editorApp);
	application().run();
	shutdownEngine();

	return 0;
}