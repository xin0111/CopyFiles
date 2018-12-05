#ifndef FILEDIRLIST_H
#define FILEDIRLIST_H

#include <QListWidget>
#include <QDropEvent>
#include <QAction>
#include <QMenu>
#include "fileslistwidget.h"

class FileDirList : public FilesListWidget
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
	bool isValidFilePath(QString& filePath);
protected:	
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *e);
	void dropEvent(QDropEvent *event);
	virtual void contextMenuEvent(QContextMenuEvent *event);
	bool typeCheck(QString fileName);
public slots:
	void ClearItems();
	void OpenDir();	
private:
	CopyDirType m_copydirType;
	QMenu   *   m_pMenu;
	QAction *   m_pOpenAction;
	QAction *   m_pDeleteItemAction;
	QAction *   m_pClearAction;
	QListWidget * m_pBuddyList;
};

#endif // FILEDIRLIST_H
