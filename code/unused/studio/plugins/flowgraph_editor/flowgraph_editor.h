#pragma once
#include "../../api/plugin.h"
#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QLayout>
#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDockWidget>
#include <QListWidget>

class PluginUI: public QObject
{
	Q_OBJECT
public:
public slots:
	void onFlowGraphEditor();
	void onResize();
};