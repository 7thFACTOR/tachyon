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
#include "project_cmds.h"

using namespace engine;

ProjectSettings g_project;

E_BEGIN_EDITOR_COMMAND(project, new)
	B_LOG_INFO("Creating new project: %s, desc: %s, class: %s, root: %s",
		pArgs->values[0].c_str(),
		pArgs->values[1].c_str(),
		pArgs->values[2].c_str(),
		pArgs->values[3].c_str());

	createFolder(pArgs->values[3]);

	JsonDocument doc;

	doc.beginDocument();
	doc.addValue("description", pArgs->values[1]);
	doc.addValue("gameManagerClass", pArgs->values[2]);
	doc.addValue("dataFolder", pArgs->values[3]);
	doc.endDocument();
	String prjFilename = (pArgs->values[3] + "/" + pArgs->values[0] + ".project");
	doc.save(prjFilename);

	editor().call("project.load", &(CmdArgs() << prjFilename));
E_END_EDITOR_COMMAND

E_BEGIN_EDITOR_COMMAND(project, load)
E_END_EDITOR_COMMAND

E_BEGIN_EDITOR_COMMAND(project, save)
E_END_EDITOR_COMMAND

E_BEGIN_EDITOR_COMMAND(project, saveAs)
E_END_EDITOR_COMMAND

E_BEGIN_EDITOR_COMMAND(project, build)
E_END_EDITOR_COMMAND

E_BEGIN_EDITOR_COMMAND(project, deploy)
E_END_EDITOR_COMMAND