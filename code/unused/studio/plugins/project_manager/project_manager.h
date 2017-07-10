#pragma once
#include "../../api/plugin.h"
#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QLayout>
#include <QMessageBox>
#include <QObject>

class PluginUI: public QObject
{
Q_OBJECT
public:
public slots:
	void onExit();
	void onNewProject();
};