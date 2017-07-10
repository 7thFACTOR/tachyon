#include "stdafx.h"
#include "editor.h"
#include <QtWidgets/QApplication>
#include "studio.h"
#include "splash_form.h"

QApplication* pApp = NULL;
SplashForm* pSplash = NULL;
EditorWindow* pMainWnd = NULL;
QProcess engineProcess;

int main(int argc, char *argv[])
{
	pApp = new QApplication(argc, argv);
	QFont font = pApp->font();
	font.setPixelSize(12);
	pApp->setFont(font);
	pApp->setStyle("fusion");
	pSplash = new SplashForm;
	pSplash->show();
	pSplash->ui.statusText->setText("Initializing...");
	pMainWnd = new EditorWindow;
	pMainWnd->setWindowTitle("[Untitled] - NytroStudio");
	pMainWnd->setStyleSheet("background-color: #353535; color: #EDEDED; highlight: #8e2dc5;} QMenu::item:selected { background-color: #54A0EB; }");
	
	QMenu* pMenu = nullptr;

	pMenu = pMainWnd->menuBar()->addMenu("&File");
	pMenu->setObjectName("file_menu");

	pMenu = pMainWnd->menuBar()->addMenu("&Edit");
	pMenu->setObjectName("edit_menu");
	pMenu->addAction("&Undo");
	pMenu->addAction("&Redo");
	pMenu->addAction("&Cut");
	pMenu->addAction("C&opy");
	pMenu->addAction("&Paste");
	pMenu->addAction("&Delete");
	pMenu->addAction("&Select all");
	pMenu->addSeparator();
	pMenu->addAction("&Preferences...");
	pMenu->addSeparator();

	pMenu = pMainWnd->menuBar()->addMenu("&Tools");
	pMenu->setObjectName("tools_menu");

	pMenu = pMainWnd->menuBar()->addMenu("&Help");
	pMenu->setObjectName("help_menu");
	pMenu->addAction("&User Guide");
	pMenu->addAction("C&heck for updates");
	pMenu->addAction("&Online Wiki");
	pMenu->addAction("&About");
	
	editor::ed.m_pMainWnd = pMainWnd;
	editor::ed.loadPluginsFromFolder("./studio_plugins");

	pMenu->addAction("&Restart engine", pMainWnd, SLOT(onRestartEngine()));

	pMainWnd->show();
	pSplash->close();

	bool bRet = pApp->exec();
	engineProcess.close();
	delete pSplash;
	delete pMainWnd;
	delete pApp;

	return bRet;
}

void startSummonedLauncher(bool bStaticLauncher)
{
	WId hWnd = pMainWnd->centralWidget()->winId();
	QStringList argList;
	QString strHandle = "--window-parent-handle=";
	char sWnd[50];

	itoa((int)hWnd, sWnd, 10);
	strHandle += sWnd;

	argList.append("--edit-mode");
	argList.append(strHandle);
	
	editor::ed.disconnectFromEngine();
	engineProcess.close();
	engineProcess.waitForFinished();

	QString processName, staticStr;

	if (bStaticLauncher)
	{
		staticStr = "_s";
	}

#ifdef _DEBUG
	processName = "launcher" + staticStr + "_d";
#else
	processName = "launcher" + staticStr;
#endif
	
	engineProcess.start(processName, argList);
	engineProcess.waitForStarted();

	if (engineProcess.state() == QProcess::ProcessState::NotRunning)
	{
		QMessageBox::warning(0, "Error", "Cannot start engine.");
	}

	editor::ed.connectToEngine();
	CALLCMD("view.resize");
	CALLCMD("view.setVisible", CmdArgs() << true);
}