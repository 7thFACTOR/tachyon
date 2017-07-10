#pragma once

#include <QDialog>
#include "ui_splash_form.h"

class SplashForm : public QDialog
{
	Q_OBJECT
public:
	SplashForm()
	{
		ui.setupUi(this);
		this->setWindowFlags(Qt::FramelessWindowHint);
	}

	~SplashForm()
	{
	}

	Ui::splashForm ui;
};