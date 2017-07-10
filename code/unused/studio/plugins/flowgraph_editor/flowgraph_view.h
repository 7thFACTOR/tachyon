#pragma once
#include <QDialog>
#include <QGraphicsItem>
#include "ui_flowgraph_view.h"

class FlowGraphNodeGraphicsItem : public QGraphicsItem
{
public:
	QRectF boundingRect() const
	{
		qreal penWidth = 1;
		return QRectF(-10 - penWidth / 2, -220 - penWidth / 2,
			120 + penWidth, 220 + penWidth);
	}

	void paint(
			QPainter *painter,
			const QStyleOptionGraphicsItem *option,
			QWidget *widget)
	{
		QBrush brush(Qt::white);
		QBrush brush2(Qt::black);
		QPen pen(brush2, .4f);
		QPen pen2(brush2, 2);
		QLinearGradient linGradient(0, 0, 0, 21);
		linGradient.setColorAt(0, Qt::black);
		linGradient.setColorAt(1, Qt::gray);
		QBrush brush3(linGradient);
		
		painter->setPen(Qt::darkGreen);
		painter->setBrush(Qt::green);
		painter->drawRoundedRect(-17, 30, 20, 10, 5, 5);

		painter->setBrush(brush);
		painter->setPen(pen);
		painter->drawRoundedRect(-10, -10, 220, 120, 3, 3);

		painter->setPen(Qt::darkGreen);
		painter->setBrush(Qt::green);
		painter->drawEllipse(-8, 30, 10, 10);
		QPainterPath path;
		path.moveTo(-17, 30);
		path.cubicTo(-30, 30, -50, 40, -150, 60);
		painter->setPen(pen2);
		painter->setBrush(Qt::NoBrush);
		painter->drawPath(path);

		painter->setPen(Qt::NoPen);
		painter->setBrush(brush3);
		painter->drawRoundedRect(-7, -7, 214, 20, 3, 3);
		painter->setBrush(brush);
		painter->setPen(Qt::white);
		painter->drawText(-7, -7, 214, 20, Qt::AlignCenter, "Expression");
	}
};

class FlowGraphBackGrid : public QGraphicsItem
{
public:
	QRectF boundingRect() const
	{
		qreal penWidth = 1;
		return QRectF(-10 - penWidth / 2, -220 - penWidth / 2,
			120 + penWidth, 220 + penWidth);
	}

	void paint(
		QPainter *painter,
		const QStyleOptionGraphicsItem *option,
		QWidget *widget)
	{
		for (int x = painter->viewport().left(); x < painter->viewport().right(); x += 10)
		{
			for (int y = painter->viewport().top(); y < painter->viewport().bottom(); y += 10)
			{
				painter->setPen(Qt::darkGray);
				painter->drawPoint(x, y);
			}
		}
	}
};

class FlowGraphView : public QWidget
{
	Q_OBJECT
public:
	FlowGraphView(QWidget* pParent)
		: QWidget(pParent)
	{
		ui.setupUi(this);
		pFlowScene = new QGraphicsScene();
		ui.graphicsView->setScene(pFlowScene);
		ui.graphicsView->setRenderHint(QPainter::Antialiasing);
		
		QPen pen(Qt::black);
		QBrush bkBrush(Qt::gray);
		
		pFlowScene->setBackgroundBrush(bkBrush);
		pFlowScene->addItem(new FlowGraphBackGrid);
		pFlowScene->addItem(new FlowGraphNodeGraphicsItem());
	}

	~FlowGraphView()
	{
	}

	QGraphicsScene* pFlowScene;

public slots:

public:
	Ui::flowGraphView ui;
};