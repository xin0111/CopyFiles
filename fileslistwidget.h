#ifndef FILESLISTWIDGET_H
#define FILESLISTWIDGET_H

#include <QListWidget>
#include <QMouseEvent>

class FilesListWidget : public QListWidget
{
	Q_OBJECT

public:
	FilesListWidget(QWidget *parent);
	~FilesListWidget();
	void setDoubleEdit(bool bEdit);
	void setkeyDelete(bool bDel);
protected:
	void keyPressEvent(QKeyEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
Q_SLOT
	void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);
public slots:
	void removeSelectItems();
private:
	bool m_doubleEdit;
	bool m_keyDelete;
};

#endif // FILESLISTWIDGET_H
