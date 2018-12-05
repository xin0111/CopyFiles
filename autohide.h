#ifndef AUTOHIDE_H
#define AUTOHIDE_H

#include <QWidget>
#include <QMouseEvent>

enum Direction
{
    Up = 0x0001,
    Left = 0x0010,
    Right = 0x0100
};
class AutoHideUI;
class AutoHide : public QWidget
{
    Q_OBJECT
public:
    explicit AutoHide(QWidget *parent = 0);
    ~AutoHide();
	void SetAttr(Direction direction, bool bIsAutoHide);
	void recordHistory(QString filePath);
signals:
	void sig_ItemDoubleClicked(QString);
	void sig_fixed(bool bFixed);
protected:
	bool eventFilter(QObject *watched, QEvent *event);
    void leaveEvent(QEvent *event);
private:
	void hideWidget();
	void showWidget();
	void addListItem(QString filePath);
	void displayHistory();
	void deleteHistory(QString value);
	void removeSelectItems();
private:
	bool m_isAutoHide;
    Direction m_enDriection;
	//父窗体的宽度
	int m_nParentWidth;
	//边界
	int m_borderOffset;
	//动画时长
	int m_nAnimaDuration;	
private://ui
	void initUi();
	AutoHideUI* ui;
};

#endif // AUTOHIDE_H
