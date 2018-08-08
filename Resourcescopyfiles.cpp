#include "copyfiles.h"
#include "Copythread.h"

CopyFilesWindow::CopyFilesWindow(QWidget *parent)
: QMainWindow(parent), m_nStep(0)
{
	ui.setupUi(this);	
	connect(CopyThread::getInstance(), SIGNAL(sig_setProgressMax(int)), this, SLOT(SetMaxProgressValue(int)));
	connect(CopyThread::getInstance(), SIGNAL(sig_addProgressStep()), this, SLOT(AddProgressStep()));
}

CopyFilesWindow::~CopyFilesWindow()
{

}


void CopyFilesWindow::on_pushButton_addPage_clicked()
{
	CopyPage* pPage = new CopyPage();
	ui.tabWidget_rule->addTab(pPage, QString::fromLocal8Bit("规则页%1").arg(ui.tabWidget_rule->count() + 1));	
}
void CopyFilesWindow::on_pushButton_deletePage_clicked()
{
	int nTabIndex = ui.tabWidget_rule->currentIndex();
	ui.tabWidget_rule->removeTab(nTabIndex);
}
void CopyFilesWindow::on_pushButton_clear_clicked()
{
	ui.tabWidget_rule->clear();
	CopyPage* pPage = new CopyPage();
	ui.tabWidget_rule->addTab(pPage, QString::fromLocal8Bit("规则页%1").arg(ui.tabWidget_rule->count() + 1));
}

void CopyFilesWindow::on_pushButton_start_clicked()
{	
	int nRule = ui.tabWidget_rule->count();
	if (nRule == 0) return;
	CopyPage * rulePage = NULL;
	QStringList fromList, toList;
	CopyThread::getInstance()->ResetFileHash();
	for (int i = 0; i < nRule; i++)
	{
		rulePage = (CopyPage*)ui.tabWidget_rule->widget(i);
		if (rulePage)
		{
			fromList = rulePage->CopyFormFilePath();
			toList = rulePage->CopyToPath();
			for each (QString var in fromList)
			{
				CopyThread::getInstance()->AddFileHash(var, toList);
			}
		}
	}
	CopyThread::getInstance()->start();
}
void CopyFilesWindow::SetMaxProgressValue(int nMax)
{
	ui.progressBar->setMaximum(nMax);
}
void CopyFilesWindow::AddProgressStep()
{
	m_nStep++;
	ui.progressBar->setValue(m_nStep);
	if (m_nStep == ui.progressBar->maximum())
	{
		m_nStep = 0;
	}
}