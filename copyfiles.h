#ifndef COPYFILES_H
#define COPYFILES_H

#include <QtWidgets/QMainWindow>
#include "ui_copyfiles.h"

class QProgressBar;
class AutoHide;
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
	void on_copyFromItemTip(QString strItem);	
	void on_reset(bool bSuccessed, QString strMsg);
	void on_pushButton_export_clicked();
	void on_pushButton_import_clicked();	
	void on_setMaxRange(int nMaxRange);
	void on_pushButton_Help_clicked();

public:
	void importFromXml(QString filePath,bool fromHistory=false);
	void exportToXml(QString filePath);
protected:
	bool eventFilter(QObject *watched, QEvent *event);
private:
	void addNewPage();	
	void resetPage();
	void registerApp();
	Ui::CopyFilesClass ui;
	int m_nStep;
	int m_nMaxRange;	
	QString m_strTitle;
	AutoHide* m_autoHide;
};

#endif // COPYFILES_H
