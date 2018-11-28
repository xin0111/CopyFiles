#ifndef AUTOHIDE_H
#define AUTOHIDE_H

#include <QWidget>
#include <QMouseEvent>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QCheckBox>
enum Direction
{
    Up = 0x0001,
    Left = 0x0010,
    Right = 0x0100
};

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
    void enterEvent(QEvent *event);	
	void keyPressEvent(QKeyEvent *event);
private:
	void addListItem(QString filePath);
    void hideWidget();
    void showWidget();
	void displayHistory();
	void deleteHistory(QString value);
	void removeSelectItems();
private:
	bool m_isAutoHide{ true };
    Direction m_enDriection;
	//父窗体的宽度
	int m_nParentWidth;
	//边界
	int m_borderOffset;
	//动画时长
	int m_nAnimaDuration;	
private://ui
	QVBoxLayout *verticalLayout;
	QListWidget *listWidget;
	QHBoxLayout *horizontalLayout;
	QSpacerItem *horizontalSpacerLeft;
	QCheckBox *checkBox;
	QSpacerItem *horizontalSpacerRight;
	void initUi();
};

#endif // AUTOHIDE_H
