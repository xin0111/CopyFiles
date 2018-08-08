#ifndef COPYFILES_H
#define COPYFILES_H

#include <QtWidgets/QMainWindow>
#include "ui_copyfiles.h"

class CopyFilesWindow : public QMainWindow
{
	Q_OBJECT

public:
	CopyFilesWindow(QWidget *parent = 0);
	~CopyFilesWindow();
public:
	void tipMessage(QString msg);
public slots:
	void on_pushButton_addPage_clicked();
	void on_pushButton_clear_clicked();
	void on_pushButton_start_clicked();
	void on_pushButton_deletePage_clicked();
	void on_reset();
	void on_pushButton_export_clicked();
	void on_pushButton_import_clicked();
	void on_showError(QString filePath);
public:
	void importFromXml(QString filePath);
	void exportToXml(QString filePath);
private:
	void addNewPage();
	Ui::CopyFilesClass ui;
	int m_nStep;
};

#endif // COPYFILES_H
