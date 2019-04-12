#include "FileDirList.h"

#include <QMimeData>
#include <QFileInfo>
#include <QDesktopServices>
#include <QDir>
#include "Copythread.h"

FileDirList::FileDirList(QWidget *parent)
: FilesListWidget(parent)
{
	this->setAcceptDrops(true);
	this->setDragEnabled(true);
	this->setContextMenuPolicy(Qt::DefaultContextMenu);	
	m_pDeleteItemAction = new QAction(QString::fromLocal8Bit("ɾ��"),this);
	connect(m_pDeleteItemAction, SIGNAL(triggered()), this, SLOT(removeSelectItems()));
	m_pClearAction = new QAction(QString::fromLocal8Bit("���"),this);
	connect(m_pClearAction, SIGNAL(triggered()), this, SLOT(ClearItems()));
	m_pOpenAction = new QAction(QString::fromLocal8Bit("���ļ���"),this);	
	connect(m_pOpenAction, &QAction::triggered, this, &FileDirList::OpenDir);
	m_pMenu = new QMenu(this);
	m_pMenu->addAction(m_pDeleteItemAction);
	m_pMenu->addAction(m_pOpenAction);
	m_pMenu->addAction(m_pClearAction);
}

FileDirList::~FileDirList()
{

}

void FileDirList::setBuddyListWidget(QListWidget * pBuddyList)
{
	m_pBuddyList = pBuddyList;
}


bool FileDirList::undoRepeat(QString fileName, bool andBuddy)
{
	//��ֹ�ظ����
	QString strShow;
	bool bRet = false;
	for (int i = 0; i < this->count(); ++i)
	{
		strShow = this->item(i)->text();
		if (strShow == fileName)
		{
			bRet = true;
			break;
		}
	}
	if (andBuddy && m_pBuddyList)
	{
		for (int i = 0; i < m_pBuddyList->count(); ++i)
		{
			strShow = m_pBuddyList->item(i)->text();
			if (strShow == fileName)
			{
				bRet = true;
				break;
			}
		}
	}
	return bRet;
}
bool FileDirList::typeCheck(QString fileName)
{
	bool bRet = false;
	QFileInfo fileInfo(fileName);
	if (m_copydirType == ALLType)
		bRet = true;
	if (m_copydirType == FilePath && fileInfo.isFile())
		bRet = true;
	if (m_copydirType == DirPath && fileInfo.isDir())
		bRet = true;
	return bRet;
}

void FileDirList::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}

void FileDirList::dragMoveEvent(QDragMoveEvent *e)
{
	e->accept();
}

void FileDirList::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat("text/uri-list"))
	{//��Դ�ļ���ק
		QList<QUrl> urls = event->mimeData()->urls();
		for (int i = 0; i < urls.size(); i++)
		{
			QString fileName = urls.at(i).toLocalFile();
			//��ֹ�ظ����
			if (typeCheck(fileName) && !undoRepeat(fileName, true))
			{
				appendItem(fileName);
			}
		}
		return;
	}
	//�ڲ���ק
	FileDirList* source = qobject_cast<FileDirList*>(event->source());

	if (!source || source == this) return;

	QList<QListWidgetItem*>& items = source->selectedItems();
	for (int i = 0; i < items.size();++i)
	{
		QListWidgetItem* pItem = items.at(i);
		QString filePath = pItem->text();
		//��ֹ�ظ����
		if (typeCheck(filePath) && !undoRepeat(filePath, source != m_pBuddyList))
		{
			appendItem(filePath);
			//ɾ��ԴItem		
			source->takeItem(source->row(pItem));
		}
	}
}

void FileDirList::ClearItems()
{
	this->clear();
}

void FileDirList::OpenDir()
{	
	QString filePath = currentItem()->text();
	if (isValidFilePath(filePath))
	{
		QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
	}
}


void FileDirList::contextMenuEvent(QContextMenuEvent *event)
{
	QListWidgetItem * pItem = currentItem();
	if (pItem)
	{
		m_pOpenAction->setVisible(isValidFilePath(currentItem()->text()));		
		m_pMenu->move(mapToGlobal(event->pos()));
		m_pMenu->show();
	}
	QListWidget::contextMenuEvent(event);
}

QListWidgetItem * FileDirList::appendItem(QString filePath)
{
	QListWidgetItem *newItem = new QListWidgetItem(this);
	newItem->setText(filePath);	
	this->insertItem(count(), newItem);		
	return newItem;
}

bool FileDirList::isValidFilePath(QString& filePath)
{
	if (QDir::isRelativePath(filePath))
	{//���·�� ���� 
		filePath = CopyThread::getInstance()->m_ruleFilePath + filePath;
	}
	int nIndex = filePath.indexOf(QRegExp("[*>]"));

	if (nIndex != -1)
	{
		filePath = filePath.mid(0, nIndex);
		QFileInfo fileInfo(filePath);
		if (fileInfo.isFile())
		{
			filePath.append("/../");
		}
		
	}
	return QFileInfo::exists(filePath);
}
