#include "common.h"
#include "core/module.h"
#include "base/defines.h"
#include "base/parameter_list.h"
#include "editor/command.h"
#include "base/platform.h"
#include "core/core.h"
#include "base/logger.h"
#include "base/json.h"
#include "editor/editor.h"
#include "render/render_manager.h"
#include "base/os_window.h"

using namespace engine;

E_BEGIN_EDITOR_COMMAND(view, setParent)
	B_LOG_INFO("Setting new parent");
	WinHandle hWnd = (WinHandle)pArgs->values[0].asInt();
	B_LOG_INFO("Setting new parent for the render window %d", (int)hWnd);
	render().osWindow()->setParent(hWnd, false);
	pReturnValues->values.append("ok");
E_END_EDITOR_COMMAND

E_BEGIN_EDITOR_COMMAND(view, resize)
	render().osWindow()->fitToParent();
E_END_EDITOR_COMMAND

E_BEGIN_EDITOR_COMMAND(view, setVisible)
	B_LOG_INFO("Setting window visibility: %d", pArgs->values[0].asBool());
	render().osWindow()->setVisible(pArgs->values[0].asBool());
E_END_EDITOR_COMMAND