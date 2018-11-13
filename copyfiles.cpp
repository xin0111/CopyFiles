#include "copyfiles.h"
#include <QMessageBox>
#include <QDomDocument>
#include <QTextStream>
#include <QFileDialog>
#include <QSet>
#include <QProgressBar>
#include <QFontMetrics>
#include <QSettings>
#include "Copythread.h"

CopyFilesWindow::CopyFilesWindow(QWidget *parent)
: QMainWindow(parent), m_nStep(0)
{
	ui.setupUi(this);	
	m_strTitle = this->windowTitle();
	ui.tabWidget_rule->setAcceptDrops(true);
	ui.tabWidget_rule->installEventFilter(this);
	ui.progressBar->setVisible(false);
	connect(CopyThread::getInstance(), SIGNAL(sig_copyFromItem(QString)), this, SLOT(on_copyFromItemTip(QString)));
	connect(CopyThread::getInstance(), SIGNAL(sig_copyRuleCount(int)), this, SLOT(on_setMaxRange(int)));
	connect(CopyThread::getInstance(), SIGNAL(sig_copyFinished(bool, QString)), this, SLOT(on_reset(bool, QString)));

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
	if (ui.tabWidget_rule->count() == 0)
	{
		this->setWindowTitle(m_strTitle);
		addNewPage();		
	}
}
void CopyFilesWindow::on_pushButton_clear_clicked()
{
	ui.tabWidget_rule->clear();
	this->setWindowTitle(m_strTitle);
	addNewPage();
}

void CopyFilesWindow::on_pushButton_start_clicked()
{	
	if (CopyThread::getInstance()->isRunning())
	{
		tipMessage(QString::fromLocal8Bit("正在拷贝中..."));
		return;
	}
	int nRule = ui.tabWidget_rule->count();
	if (nRule == 0) return;
	CopyPage * rulePage = NULL;
	QStringList fromList, toList;
	CopyThread::getInstance()->ResetFileRules();
	m_nMaxRange = 0;	m_nStep = 0;
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
			}
		}
	}
	CopyThread::getInstance()->start();
	ui.pushButton_start->setText(QString::fromLocal8Bit("正在查找文件..."));

}

void CopyFilesWindow::on_setMaxRange(int nMaxRange)
{
	m_nMaxRange = nMaxRange;
	ui.progressBar->setRange(0, m_nMaxRange);
	ui.progressBar->setVisible(true);
	ui.pushButton_start->setText(QString::fromLocal8Bit("正在拷贝..."));
}

void CopyFilesWindow::on_copyFromItemTip(QString strItem)
{
	ui.progressBar->setValue(m_nStep);

	QFont ft;
	QFontMetrics fm(ft);
	strItem = fm.elidedText(strItem, Qt::ElideMiddle, ui.label_progress->width());
	ui.label_progress->setText(strItem);
	m_nStep += 1;
}

void CopyFilesWindow::on_reset(bool bSuccessed, QString strMsg)
{
	ui.pushButton_start->setText(QString::fromLocal8Bit("开始复制"));
	ui.progressBar->setValue(0);
	ui.progressBar->setVisible(false);
	ui.label_progress->setText("");
	if (!bSuccessed)
		tipMessage(strMsg);
	tipMessage(bSuccessed ? QString::fromLocal8Bit("复制完成.") : 
		QString::fromLocal8Bit("复制终止."));
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
	QFileInfo fileInfo(filePath);
	this->setWindowTitle(fileInfo.fileName() + " - " + m_strTitle);
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

	QFileInfo fileInfo(filePath);
	this->setWindowTitle(fileInfo.fileName() + " - " + m_strTitle);
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
	{
		importFromXml(fileName);
	}
}

void CopyFilesWindow::tipMessage(QString msg)
{
	QMessageBox::information(this,QString::fromLocal8Bit("提示"),msg);
}

void CopyFilesWindow::addNewPage()
{
	CopyPage* pPage = new CopyPage(this);	
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
	int nValue = -1;
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

	nIndex = (nValue != -1 ) ? nValue : nCount;

	int addIndex = ui.tabWidget_rule->addTab(pPage,
		QString::fromLocal8Bit("规则页%1").arg(nIndex + 1));
	ui.tabWidget_rule->setCurrentIndex(addIndex);
}

void CopyFilesWindow::on_pushButton_Help_clicked()
{
	QMessageBox msgBox;
	msgBox.setInformativeText(QString::fromLocal8Bit("<p> 特定正则使用规则:</p>"
		"<p>   + :  <b>（目录/*.+）或（文件+）或（*.后缀+）</b> 自动创建拷贝根目录；</p >"
		"<p>   - :  <b>（目录/*.-）或（文件-）或（*.后缀-）</b>  只拷贝根目录下的匹配项。</p>"
		"<p>   > :  <b>（目录/>新目录）或（文件>新目录）或（*.后缀>新目录）</b> 创建拷贝新根目录；</p >"
		));
	msgBox.setWindowIcon(this->windowIcon());
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.addButton(QStringLiteral("注册"), QMessageBox::AcceptRole);
	msgBox.setDefaultButton(QMessageBox::Ok);
	int ret = msgBox.exec();
	switch (ret)
	{
	case QMessageBox::AcceptRole:
		registerApp();
	default:
		break;
	}
}

bool CopyFilesWindow::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == ui.tabWidget_rule)
	{
		if (event->type() == QEvent::DragEnter)
		{
			event->accept();
		}
		if (event->type() == QEvent::DragMove)
		{
			int nIndex = ui.tabWidget_rule->tabBar()->tabAt(((QDragMoveEvent*)event)->pos());
			if (nIndex != -1)
			{
				ui.tabWidget_rule->tabBar()->setCurrentIndex(nIndex);
			}
			event->accept();
		}
	}
	return QMainWindow::eventFilter(watched, event);
}

void CopyFilesWindow::registerApp()
{
	//file
	QSettings regFile("HKEY_CLASSES_ROOT\\*\\shell\\CopyFiles", QSettings::NativeFormat);
	regFile.setValue("icon", QCoreApplication::arguments()[0]); //设置注册表值
	QSettings regFile2("HKEY_CLASSES_ROOT\\*\\shell\\CopyFiles\\command", QSettings::NativeFormat);
	regFile2.setValue("Default", QCoreApplication::arguments()[0] + " -f " + "%1");
	//directory
	QSettings regDirectory("HKEY_CLASSES_ROOT\\directory\\background\\shell\\CopyFiles", QSettings::NativeFormat);
	regDirectory.setValue("icon", QCoreApplication::arguments()[0]);
	QSettings regDirectory2("HKEY_CLASSES_ROOT\\directory\\background\\shell\\CopyFiles\\command", QSettings::NativeFormat);
	regDirectory2.setValue("Default", QCoreApplication::arguments()[0]);

}
