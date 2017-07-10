#include "project_manager.h"
#include "new_project_form.h"
#include "dashboard_form.h"

NSTUDIO_BEGIN_PLUGIN
PluginUI* ui;
bool onLoad()
{
	ui = new PluginUI();
	QMenu* pMenu;
		
	pMenu = (QMenu*)ed->mainWindow()->menuBar()->findChild<QMenu*>("file_menu");

	if (pMenu)
	{
		QAction* pNewProjectAction = pMenu->addAction("New project...");
		QAction* pOpenProjectAction = pMenu->addAction("Set current project...");
		pMenu->addSeparator();
		QAction* pBuildProjectAction = pMenu->addAction("Build project...");
		QAction* pDeployProjectAction = pMenu->addAction("Deploy project...");
		pMenu->addSeparator();
		QAction* pExitAction = pMenu->addAction("&Exit");
		
		QObject::connect(pExitAction, SIGNAL(triggered()), ui, SLOT(onExit()));
		QObject::connect(pNewProjectAction, SIGNAL(triggered()), ui, SLOT(onNewProject()));
	}

	DashboardForm* pDashboard = new DashboardForm();
	QHBoxLayout* pLayout = new QHBoxLayout(ed->mainWindow()->centralWidget());

	ed->mainWindow()->centralWidget()->setLayout(pLayout);
	ed->mainWindow()->centralWidget()->layout()->addWidget(pDashboard);
	pDashboard->setParent(ed->mainWindow()->centralWidget());
	pDashboard->show();

	return true;
}

bool onUnload()
{
	delete ui;
	return true;
}

NSTUDIO_END_PLUGIN

void PluginUI::onExit()
{
	ed->mainWindow()->close();
}

void PluginUI::onNewProject()
{
	NewProjectForm frm(ed->mainWindow());
	
	frm.exec();
	ed->call("project.new", 
						CmdArgs() 
						<< frm.ui.edProjectName->text().toLatin1().constData()
						<< frm.ui.edProjectDesc->text().toLatin1().constData()
						<< frm.ui.edProjectClassname->currentText().toLatin1().constData()
						<< frm.ui.edRootFolder->text().toLatin1().constData());
}