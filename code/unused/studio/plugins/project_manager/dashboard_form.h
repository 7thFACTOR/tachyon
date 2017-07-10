#pragma once
#include <QDialog>
#include "ui_dashboard_form.h"

class DashboardForm: public QDialog
{
	Q_OBJECT
public:
	DashboardForm(): QDialog()
	{
		ui.setupUi(this);
	}

	~DashboardForm()
	{
	}

public slots:

public:
	Ui::dashboardForm ui;
};