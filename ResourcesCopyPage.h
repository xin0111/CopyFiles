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
	QStringList CopyFormFilePath();
	QStringList CopyToPath();
private:
	Ui_CopyPage ui;
};

#endif // COPYPAGE_H
