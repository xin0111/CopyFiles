#include "copyfiles.h"
#include <QMessageBox>
#include <QDomDocument>
#include <QTextStream>
#include <QFileDialog>
#include <QSet>
#include "Copythread.h"

CopyFilesWindow::CopyFilesWindow(QWidget *parent)
: QMainWindow(parent), m_nStep(0)
{
	ui.setupUi(this);	
	connect(CopyThread::getInstance(), SIGNAL(sig_copySuccessed()), this, SLOT(on_reset()));
	connect(CopyThread::getInstance(), SIGNAL(sig_errorfilePath(QString)), this, SLOT(on_showError(QString)));
}

CopyFilesWindow::~CopyFilesWindow()
{

}


void CopyFilesWindow::on_pushButton_addPage_clicked()
{
	addNewPage();
}
void CopyFilesWindow::on_pushButton_deletePage_clicked()
{
	int nTabIndex = ui.tabWidget_rule->currentIndex();
	ui.tabWidget_rule->removeTab(nTabIndex);
	if (nTabIndex == 0)
		addNewPage();
}
void CopyFilesWindow::on_pushButton_clear_clicked()
{
	ui.tabWidget_rule->clear();
	addNewPage();
}

void CopyFilesWindow::on_pushButton_start_clicked()
{	
	int nRule = ui.tabWidget_rule->count();
	if (nRule == 0) return;
	CopyPage * rulePage = NULL;
	QStringList fromList, toList;
	CopyThread::getInstance()->ResetFileHash();
	bool hasRule = false;
	for (int i = 0; i < nRule; i++)
	{
		rulePage = (CopyPage*)ui.tabWidget_rule->widget(i);
		if (rulePage)
		{
			if (!rulePage->GetRuleEnable()) continue;
			fromList = rulePage->CopyFormFilePath();
			toList = rulePage->CopyToPath();
			if (toList.isEmpty())
				continue;
			for each (QString var in fromList)
			{
				CopyThread::getInstance()->AddFileHash(var, toList);
				hasRule = true;
			}
		}
	}
	if (hasRule)
	{
		CopyThread::getInstance()->start();
		ui.pushButton_start->setText(QString::fromLocal8Bit("正在拷贝..."));
	}
	else
	{
		tipMessage(QString::fromLocal8Bit("不存在正确规则!"));
	}
}

void CopyFilesWindow::on_reset()
{
	ui.pushButton_start->setText(QString::fromLocal8Bit("开始复制"));
	tipMessage(QString::fromLocal8Bit("复制完成."));
}

void CopyFilesWindow::importFromXml(QString filePath)
{
	QDomDocument doc;
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
		return;
	if (!doc.setContent(&file)) {
		file.close();
		return;
	}
	file.close();

	QDomElement root = doc.documentElement();
	QDomNodeList pages = root.childNodes();
	ui.tabWidget_rule->clear();
	CopyPage * rulePage = NULL;
	for (size_t i = 0; i < pages.size(); i++)
	{
		on_pushButton_addPage_clicked();
		rulePage = (CopyPage*)ui.tabWidget_rule->widget(i);
		if (rulePage == NULL) continue;
		QDomNode pageNode = pages.at(i);
		rulePage->SetRuleEnable(pageNode.toElement().attribute("Enable").toInt());
		QDomNodeList fromNodes = pageNode.firstChildElement("From").childNodes();
		for (size_t j = 0; j < fromNodes.size(); ++j)
		{
			rulePage->AddCopyFromPath(fromNodes.at(j).toElement().text());
		}
		QDomNodeList toNodes = pageNode.firstChildElement("To").childNodes();
		for (size_t k = 0; k < toNodes.size(); ++k)
		{
			rulePage->AddCopyToPath(toNodes.at(k).toElement().text());
		}
	}
}

void CopyFilesWindow::exportToXml(QString filePath)
{
	int nRule = ui.tabWidget_rule->count();
	if (nRule == 0) return;
	CopyPage * rulePage = NULL;
	QStringList fromList, toList;
	
	QDomDocument doc;
	QDomElement root = doc.createElement("Rule");
	for (int i = 0; i < nRule; i++)
	{
		rulePage = (CopyPage*)ui.tabWidget_rule->widget(i);
		if (rulePage)
		{
			QDomElement pageEle = doc.createElement("Page");
			fromList = rulePage->CopyFormFilePath();
			toList = rulePage->CopyToPath();
			pageEle.setAttribute("Enable", QString::number(rulePage->GetRuleEnable()));
			if (toList.isEmpty())
				continue;
			QDomElement fromEle = doc.createElement("From");
			QDomElement toEle = doc.createElement("To");
			for each (QString var in fromList)
			{
				QDomElement node = doc.createElement("Node");
				QDomText text = doc.createTextNode(var);
				node.appendChild(text);
				fromEle.appendChild(node);
			}
			pageEle.appendChild(fromEle);
			for each (QString var in toList)
			{				
				QDomElement node = doc.createElement("Node");
				QDomText text = doc.createTextNode(var);
				node.appendChild(text);
				toEle.appendChild(node);
			}
			pageEle.appendChild(toEle);
			root.appendChild(pageEle);
		}
	}
	doc.appendChild(root);

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly|QFile::Truncate))
		return;
	QTextStream ts(&file);
	ts.reset();
	doc.save(ts, 4, QDomNode::EncodingFromTextStream);
	file.close();
	tipMessage(QString::fromLocal8Bit("导出完成."));
}



void CopyFilesWindow::on_pushButton_export_clicked()
{
	QString filePath = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存文件"),
		"CopyRule.xml",	"Xml Files (*.xml )");
	if (!filePath.isEmpty())
	{		
		exportToXml(filePath);
	}
}

void CopyFilesWindow::on_pushButton_import_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open File"), "", tr("Xml Files (*.xml )"));
	if (!fileName.isEmpty())
		importFromXml(fileName);
}

void CopyFilesWindow::tipMessage(QString msg)
{
	QMessageBox box;
	box.setText(msg);
	box.exec();
}

void CopyFilesWindow::on_showError(QString filePath)
{
	QString strMsg = QString::fromLocal8Bit("不存在文件(夹)\n");
	strMsg.append(filePath);
	tipMessage(strMsg);
}

void CopyFilesWindow::addNewPage()
{
	CopyPage* pPage = new CopyPage();	
	QRegExp rx("\\d+");
	QList<int> nNums;
	int nCount = ui.tabWidget_rule->count();
	for (int i = 0; i < nCount; ++i)
	{
		QString str = ui.tabWidget_rule->tabText(i);
		int pos = rx.indexIn(str);
		QString strValue = str.mid(pos);
		nNums << QVariant(strValue).toInt();
	
	}
	int nValue = 0;
	int nIndex = 0;
	qSort(nNums);
	for (int i = 0; i < nNums.size();++i)
	{
		if ((i + 1) != nNums.at(i))
		{
			nValue = i;	
			break;
		}
	}

	nIndex = (nValue != 0 ) ? nValue : nCount;

	int addIndex = ui.tabWidget_rule->addTab(pPage,
		QString::fromLocal8Bit("规则页%1").arg(nIndex + 1));
	ui.tabWidget_rule->setCurrentIndex(addIndex);
}

