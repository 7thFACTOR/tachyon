#include "new_project_form.h"
#include <QMessageBox>
#include <QFileDialog>

void NewProjectForm::onCreateProject()
{
	if (ui.edProjectName->text().isEmpty())
	{
		ui.edProjectName->setFocus();
		return;
	}

	if (ui.edRootFolder->text().isEmpty())
	{
		ui.edRootFolder->setFocus();
		return;
	}

	this->done(0);
}

void NewProjectForm::onCancel()
{
	this->done(0);
}

void NewProjectForm::onBrowseRoot()
{
	QFileDialog dlg;

	dlg.setFileMode(QFileDialog::DirectoryOnly);
	dlg.exec();
	ui.edRootFolder->setText(dlg.directory().absolutePath());
}
