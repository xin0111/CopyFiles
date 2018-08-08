#ifndef COPYPAGE_H
#define COPYPAGE_H

#include <QWidget>
#include "ui_CopyPage.h"

class CopyPage : public QWidget
{
	Q_OBJECT

public:
	CopyPage(QWidget *parent = 0);
	~CopyPage();
public:
	void AddCopyFromPath(QString filepath);
	void AddCopyToPath(QString filepath);
	QStringList CopyFormFilePath();
	QStringList CopyToPath();
	bool GetRuleEnable();	
	void SetRuleEnable(bool bState);
private:
	Ui_CopyPage ui;
};

#endif // COPYPAGE_H
