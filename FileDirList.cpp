#include "FileDirList.h"

#include <QMimeData>
#include <QFileInfo>
#include <QDesktopServices>

FileDirList::FileDirList(QWidget *parent)
: QListWidget(parent)
{
	this->setAcceptDrops(true);
	this->setDragEnabled(true);
	this->setContextMenuPolicy(Qt::DefaultContextMenu);
	m_pDeleteItemAction = new QAction(QString::fromLocal8Bit("ɾ��"));
	connect(m_pDeleteItemAction, SIGNAL(triggered()), this, SLOT(RemoveSelectItem()));
	m_pClearAction = new QAction(QString::fromLocal8Bit("���"));
	connect(m_pClearAction, SIGNAL(triggered()), this, SLOT(ClearItems()));
	m_pOpenAction = new QAction(QString::fromLocal8Bit("���ļ���"));
	connect(m_pOpenAction, SIGNAL(triggered()), this, SLOT(OpenDir()));	
	m_pMenu = new QMenu;
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
	if (m_copydirType == FilePath && fileInfo.isFile())
		bRet = true;
	if (m_copydirType == DirPath && fileInfo.isDir())
		bRet = true;
	if (m_copydirType == ALLType && fileInfo.exists())
		bRet = true;
	return bRet;
}
void FileDirList::dragEnterEvent(QDragEnterEvent *event)
{	
	if (event->mimeData()->hasFormat("text/uri-list"))
	{
		QList<QUrl> urls = event->mimeData()->urls();
		if (!urls.isEmpty())
		{			
			for (int i = 0; i < urls.size(); i++)
			{
				QString fileName = urls.at(i).toLocalFile();
				//��ֹ�ظ����
				if (typeCheck(fileName) && !undoRepeat(fileName, true))
				{
					appendItem(fileName);
				}				
			}
		}
	}
	event->accept();
}
void FileDirList::dropEvent(QDropEvent *event)
{
	FileDirList* source = qobject_cast<FileDirList*>(event->source());
	if (source == this || m_pBuddyList == NULL) return;
	if (source && source == m_pBuddyList)
	{
		QString filePath = m_pBuddyList->currentItem()->text();
		//��ֹ�ظ����
		if (typeCheck(filePath) && !undoRepeat(filePath, false))
		{
			appendItem(filePath);
			//ɾ��ԴItem
			m_pBuddyList->takeItem(m_pBuddyList->currentRow());
		}
	}

	//QListWidget::dropEvent(event);
}

void FileDirList::RemoveSelectItem()
{
	int nRowIndex = this->currentRow();
	takeItem(nRowIndex);
}

void FileDirList::ClearItems()
{
	this->clear();
}

void FileDirList::OpenDir()
{	
	QString fielPath = currentItem()->text();
	QFileInfo fileInfo(fielPath);
	if (fileInfo.isFile()){
		fielPath.append("/../");
	}
	QDesktopServices::openUrl(QUrl::fromLocalFile(fielPath));
}


void FileDirList::contextMenuEvent(QContextMenuEvent *event)
{
	QListWidgetItem * pItem = currentItem();
	if (pItem)
	{
		QFileInfo fileInfo(currentItem()->text());
		m_pOpenAction->setVisible(fileInfo.exists());		
		m_pMenu->move(mapToGlobal(event->pos()));
		m_pMenu->show();
	}
	QListWidget::contextMenuEvent(event);
}

QListWidgetItem * FileDirList::appendItem(QString filePath)
{
	QListWidgetItem *newItem = new QListWidgetItem;
	newItem->setText(filePath);
	//���ÿɱ༭ 
	newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
	newItem->setSelected(true);
	this->insertItem(count(), newItem);		
	return newItem;
}

void FileDirList::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Delete)
	{
		takeItem(currentRow());
	}
}

void FileDirList::mouseDoubleClickEvent(QMouseEvent *event)
{	
	QPoint pos = event->pos();
	QListWidgetItem * pItem = itemAt(pos.x(), pos.y());
	if (pItem)
	{
		editItem(pItem);
	}
	else
	{
		this->addItem("");
		pItem = item(count() - 1);
		if (pItem)
		{
			pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
			setCurrentItem(pItem);			
			editItem(pItem);			
		}
	}
}
#include <QMetaProperty>
void FileDirList::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
	QMap<int, QVariant> datas = this->model()->itemData(currentIndex());
	if (datas.isEmpty()) return;
	QString path = datas.first().toString();
	if (path.isEmpty())
	{
		takeItem(currentRow());
		return;
	}
	else
	{
		//TODO
	}
	QListWidget::closeEditor(editor, hint);
}
