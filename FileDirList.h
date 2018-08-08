#ifndef FILEDIRLIST_H
#define FILEDIRLIST_H

#include <QListWidget>
#include <QDropEvent>
#include <QAction>
#include <QMenu>
class FileDirList : public QListWidget
{
	Q_OBJECT

public:
	enum CopyDirType
	{
		FilePath,
		DirPath,
		ALLType
	};
	FileDirList(QWidget *parent);
	~FileDirList();
public:
	void setCopyDirType(CopyDirType type){ m_copydirType = type; }
	CopyDirType getCopyDirType(){ return m_copydirType; }
	void setBuddyListWidget(QListWidget * pBuddyList);	
	bool undoRepeat(QString fileName, bool andBuddy);
	QListWidgetItem * appendItem(QString filePath);
protected:
	void dropEvent(QDropEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	virtual void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	bool typeCheck(QString fileName);
public slots:
	void RemoveSelectItem();
	void ClearItems();
	void OpenDir();	
Q_SLOT
	void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);

private:
	CopyDirType m_copydirType;
	QMenu   *   m_pMenu;
	QAction *   m_pOpenAction;
	QAction *   m_pDeleteItemAction;
	QAction *   m_pClearAction;
	QListWidget * m_pBuddyList;
};

#endif // FILEDIRLIST_H
