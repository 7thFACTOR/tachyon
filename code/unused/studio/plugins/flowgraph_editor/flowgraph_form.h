#pragma once
#include <QDialog>
#include <QGraphicsItem>
#include "ui_flowgraph_form.h"
#include "flowgraph_view.h"

class FlowGraphForm : public QWidget
{
	Q_OBJECT
public:
	FlowGraphForm(QWidget* pParent)
		: QWidget(pParent)
	{
		ui.setupUi(this);
		ui.tabWidget->addTab(new FlowGraphView(ui.tabWidget), "flow01.flowgraph");
	}

	~FlowGraphForm()
	{
	}

	QGraphicsScene* pFlowScene;

public slots:

public:
	Ui::flowGraphForm ui;
};