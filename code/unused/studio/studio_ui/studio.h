#pragma once
#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <ui_studio.h>

class EditorWindow : public QMainWindow
{
	Q_OBJECT

public:
	EditorWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~EditorWindow();
signals:
	void signalResize(QResizeEvent *);

protected:
	void resizeEvent(QResizeEvent *);  // virtual
public slots:
	void resized(QResizeEvent *);
	void onCheckCrashTimer();
	void onBtnRestartEngine();
	void onRestartEngine();
private:
	Ui::editorClass ui;
	QTimer m_crashCheckTimer;
};

extern QProcess engineProcess;

void startSummonedLauncher(bool bStaticLauncher = false);
void showDashboard();
