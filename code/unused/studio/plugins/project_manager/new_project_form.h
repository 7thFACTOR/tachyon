#pragma once
#include <QDialog>
#include "ui_new_project_form.h"

class NewProjectForm : public QDialog
{
	Q_OBJECT
public:
	NewProjectForm(QWidget* pParent)
		: QDialog(pParent)
	{
		ui.setupUi(this);
		this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint|Qt::Dialog);
		connect(ui.btnBrowseRoot, SIGNAL(clicked()), this, SLOT(onBrowseRoot()));
		connect(ui.btnCreate, SIGNAL(clicked()), this, SLOT(onCreateProject()));
		connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
	}

	~NewProjectForm()
	{
	}

public slots:
	void onCreateProject();
	void onCancel();
	void onBrowseRoot();

public:
	Ui::newProjectForm ui;
};