#include "entity_editor.h"

NSTUDIO_BEGIN_PLUGIN
PluginUI* ui;
bool onLoad()
{
	ui = new PluginUI;
	QMenu* pMenu = ed->mainWindow()->menuBar()->findChild<QMenu*>("tools_menu");

	if (pMenu)
	{
		pMenu->addAction("&Entity editor");
	}

	return true;
}

bool onUnload()
{
	delete ui;
	return true;
}

NSTUDIO_END_PLUGIN

