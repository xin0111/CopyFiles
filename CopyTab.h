#ifndef COPYTAB_H
#define COPYTAB_H

#include <QTabWidget>

class CopyTab : public QTabWidget
{
	Q_OBJECT

public:
	CopyTab(QWidget *parent = Q_NULLPTR);
	~CopyTab();
protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *e);
	void dropEvent(QDropEvent *event);
private:
	
};

#endif // COPYTAB_H
