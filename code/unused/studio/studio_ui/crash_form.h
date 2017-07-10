#pragma once

#include <QDialog>
#include "ui_crash_form.h"

class CrashForm : public QDialog
{
	Q_OBJECT
public:
	CrashForm()
	{
		ui.setupUi(this);
		this->setWindowFlags(Qt::FramelessWindowHint);
	}

	~CrashForm()
	{
	}

	Ui::crashForm ui;
};