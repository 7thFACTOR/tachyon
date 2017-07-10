#include "flowgraph_editor.h"
#include "flowgraph_form.h"

NSTUDIO_BEGIN_PLUGIN
PluginUI* ui;

bool onLoad()
{
	ui = new PluginUI;
	auto pMenu = (QMenu*)ed->mainWindow()->menuBar()->findChild<QMenu*>("tools_menu");

	if (pMenu)
	{
		QAction* pFlowGraphAction = pMenu->addAction("FlowGraph editor...");
		QObject::connect(pFlowGraphAction, SIGNAL(triggered()), ui, SLOT(onFlowGraphEditor()));
	}
	
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
}

void PluginUI::onFlowGraphEditor()
{
	QDockWidget* docker = new QDockWidget(tr("FlowGraph"), ed->mainWindow());
	FlowGraphForm* pForm = new FlowGraphForm(docker);
	docker->setAllowedAreas(Qt::AllDockWidgetAreas);
	docker->setWidget(pForm);
	docker->setVisible(true);
	docker->setFeatures(
		docker->features() |
		QDockWidget::DockWidgetMovable
		| QDockWidget::DockWidgetClosable
		| QDockWidget::DockWidgetFloatable);

	ed->mainWindow()->addDockWidget(Qt::RightDockWidgetArea, docker, Qt::Vertical);

}