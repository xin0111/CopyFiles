#include "CopyPage.h"

CopyPage::CopyPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);	
	ui.splitter->setStretchFactor(0, 2);
	ui.splitter->setStretchFactor(1, 1);	
	ui.listWidget_copyFrom->setCopyDirType(FileDirList::ALLType);
	ui.listWidget_copyFrom->setBuddyListWidget(ui.listWidget_copyTo);
	ui.listWidget_copyTo->setCopyDirType(FileDirList::DirPath);
	ui.listWidget_copyTo->setBuddyListWidget(ui.listWidget_copyFrom);
}

CopyPage::~CopyPage()
{
	
}


QStringList CopyPage::CopyFormFilePath()
{
	QStringList strList;
	int nRow = ui.listWidget_copyFrom->count();
	for (int i = 0; i < nRow; i++)
	{
		strList << ui.listWidget_copyFrom->item(i)->text();
	}
	return strList;
}
QStringList CopyPage::CopyToPath()
{
	QStringList strList;
	int nRow = ui.listWidget_copyTo->count();
	for (int i = 0; i < nRow; i++)
	{
		strList << ui.listWidget_copyTo->item(i)->text();
	}
	return strList;
}

void CopyPage::AddCopyFromPath(QString filepath)
{
	ui.listWidget_copyFrom->appendItem(filepath);
}

void CopyPage::AddCopyToPath(QString filepath)
{
	ui.listWidget_copyTo->appendItem(filepath);
}

bool CopyPage::GetRuleEnable()
{
	return ui.groupBox_From->isChecked();
}
void CopyPage::SetRuleEnable(bool bState)
{
	ui.groupBox_From->setChecked(bState);
}


