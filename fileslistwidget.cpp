#include "fileslistwidget.h"
#include <QMetaProperty>

FilesListWidget::FilesListWidget(QWidget *parent)
	: QListWidget(parent)
{
	m_doubleEdit = true;
	m_keyDelete = true;
	this->setSelectionMode(QListWidget::ExtendedSelection);
}

FilesListWidget::~FilesListWidget()
{

}

void FilesListWidget::setDoubleEdit(bool bEdit)
{
	m_doubleEdit = bEdit;
}

void FilesListWidget::setkeyDelete(bool bDel)
{
	m_keyDelete = bDel;
}
void FilesListWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case  Qt::Key_Delete:
	{
							if (m_keyDelete)
								removeSelectItems();
	}
		break;
	case Qt::Key_Up:
	{
					   int nCurrentRow = currentRow();
					   if (nCurrentRow != 0 &&
						   nCurrentRow != -1)
					   {
						   int nPreRow = nCurrentRow - 1;

						   QListWidgetItem * pCurrentItem = item(nCurrentRow);
						   QListWidgetItem * pPreItem = item(nPreRow);
						   QString strCurrent = pCurrentItem->text();
						   QString strPre = pPreItem->text();

						   pCurrentItem->setText(strPre);
						   pPreItem->setText(strCurrent);

						   setCurrentRow(nPreRow);
					   }

	}break;
	case Qt::Key_Down:
	{
						 int nCurrentRow = currentRow();
						 if (nCurrentRow != count() - 1 &&
							 nCurrentRow != -1)
						 {
							 int nNextRow = nCurrentRow + 1;

							 QListWidgetItem * pCurrentItem = item(nCurrentRow);
							 QListWidgetItem * pNextItem = item(nNextRow);

							 QString strCurrent = pCurrentItem->text();
							 QString strNext = pNextItem->text();

							 pCurrentItem->setText(strNext);
							 pNextItem->setText(strCurrent);

							 setCurrentRow(nNextRow);
						 }
	}break;
	default:
		break;
	}
}

void FilesListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (!m_doubleEdit) 
		QListWidget::mouseDoubleClickEvent(event);
	else
	{
		QPoint pos = event->pos();
		QListWidgetItem * pItem = itemAt(pos.x(), pos.y());
		if (pItem)
		{
			pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
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
}

void FilesListWidget::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
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

void FilesListWidget::removeSelectItems()
{
	QList<QListWidgetItem*>& indexList = this->selectedItems();
	for (int i = 0; i < indexList.size(); ++i)
	{
		this->takeItem(this->row(indexList.at(i)));
	}
	this->setCurrentRow(count() - 1);
}