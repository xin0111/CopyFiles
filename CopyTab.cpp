#include "CopyTab.h"
#include "qevent.h"
#include "qtabbar.h"

CopyTab::CopyTab(QWidget *parent)
	: QTabWidget(parent)
{
	this->setAcceptDrops(true);
}

CopyTab::~CopyTab()
{

}

void CopyTab::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
	QTabWidget::dragEnterEvent(event);
}

void CopyTab::dragMoveEvent(QDragMoveEvent *e)
{
	int nIndex = tabBar()->tabAt(e->pos());
	if (nIndex != -1)
	{
		tabBar()->setCurrentIndex(nIndex);
	}
	QTabWidget::dragMoveEvent(e);
}

void CopyTab::dropEvent(QDropEvent *event)
{
	QTabWidget::dropEvent(event);
}


