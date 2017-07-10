#include "render_viewport.h"

NSTUDIO_BEGIN_PLUGIN
PluginUI* ui;
bool onLoad()
{
	ui = new PluginUI;
	//ed->call("view.setParent", CmdArgs()
	//	<< (int)ed->mainWindow()->centralWidget()->winId());
	ed->call("view.setVisible", CmdArgs()
		<< true);

	return true;
}

bool onUnload()
{
	delete ui;
	return true;
}
NSTUDIO_END_PLUGIN

void PluginUI::onResize()
{
	ed->call("view.resize");
}