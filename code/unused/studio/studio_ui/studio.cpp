#include "stdafx.h"
#include "studio.h"
#include "editor.h"
#include "crash_form.h"

EditorWindow::EditorWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	this->setWindowState(Qt::WindowMaximized);
	connect(this, SIGNAL(signalResize(QResizeEvent *)), 
		this, SLOT(resized(QResizeEvent *)));

	m_crashCheckTimer.setParent(this);
	connect(&m_crashCheckTimer, SIGNAL(timeout()), this, SLOT(onCheckCrashTimer()));
	//m_crashCheckTimer.start(1000);
}

EditorWindow::~EditorWindow()
{
}

void EditorWindow::resizeEvent(QResizeEvent *e)
{
	// call the base method
	QMainWindow::resizeEvent(e);

	emit signalResize(e);
}

void EditorWindow::resized(QResizeEvent *ev)
{
	CALLCMD("view.resize",
		CmdArgs() << ev->size().width() << ev->size().height());
}

void EditorWindow::onCheckCrashTimer()
{
	if (engineProcess.state() == QProcess::NotRunning)
	{
		m_crashCheckTimer.stop();
		QHBoxLayout* pLayout = new QHBoxLayout(centralWidget());

		centralWidget()->setLayout(pLayout);
		CrashForm* pFrm = nullptr;
		
		pFrm = new CrashForm();
		connect(pFrm->ui.btnRestartEngine, SIGNAL(pressed()), this, SLOT(onBtnRestartEngine()));

		if (centralWidget() && centralWidget()->layout())
		{
			centralWidget()->layout()->addWidget(pFrm);
		}
	}
}

void EditorWindow::onBtnRestartEngine()
{
	QLayout* pLayout = centralWidget()->layout();

	if (pLayout)
	{
		int i = 0;
		QLayoutItem *child;
	
		while ((child = pLayout->takeAt(0)) != 0)
		{
			delete child->widget();
			delete child;
		}
	}

	startSummonedLauncher();
	m_crashCheckTimer.start(1000);
}

void EditorWindow::onRestartEngine()
{
	m_crashCheckTimer.stop();
	onBtnRestartEngine();
}