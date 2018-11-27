#include "autohide.h"
#include <QPoint>
#include <QDebug>
#include <QSettings>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>

#define  AUTOHIDE_BASEREG "HKEY_CURRENT_USER\\Software\\CopyFiles"
#define  AUTOHIDE_GROUP "Recent Document List"
#define  AUTOHIDE_DOCUMENT "Document"
#define  AUTOHIDE_DOCUMENTCOUNT "DocumentCount"

AutoHide::AutoHide(QWidget *parent) :
QWidget(parent)
{
	initUi();
    m_nParentWidth = parent->width();
	parent->installEventFilter(this);
	m_enDriection = Left;
	m_borderOffset = 18;
	m_nAnimaDuration = 300;
	displayHistory();

#if 0   //阴影效果
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(5);
    effect->setColor(Qt::black);
    effect->setOffset(0,0);
    setGraphicsEffect(effect);
#endif
}

AutoHide::~AutoHide()
{
  
}

void AutoHide::SetAttr(Direction direction, bool bIsAutoHide)
{
	m_enDriection = direction;
	m_isAutoHide = bIsAutoHide;
}

void AutoHide::recordHistory(QString filePath)
{
	QSettings settings(AUTOHIDE_BASEREG, QSettings::NativeFormat);
	settings.beginGroup(AUTOHIDE_GROUP);
	
	QStringList child = settings.childKeys();
	int nCount = settings.value(AUTOHIDE_DOCUMENTCOUNT).toInt();
	settings.setValue(AUTOHIDE_DOCUMENT + QString::number(++nCount), filePath);
	settings.setValue(AUTOHIDE_DOCUMENTCOUNT, nCount);
	settings.endGroup();
	//显示
	QListWidgetItem* item = new QListWidgetItem(filePath);
	item->setSizeHint(QSize(this->width(), 100));
	listWidget->addItem(item);
}

bool AutoHide::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == this->parent())
	{
		if (event->type() == QEvent::Resize)
		{
			QWidget* parent = qobject_cast<	QWidget*>(watched);

			if (Up == m_enDriection)
			{
				this->resize(parent->width() *0.8, parent->height()*0.7);
				m_nParentWidth = parent->width();
				move(m_nParentWidth / 10, -this->height() + m_borderOffset);
			}
			else if (Left == m_enDriection)
			{
				this->resize(parent->width() *0.5, parent->height() - 34);
				m_nParentWidth = parent->width();
				move(-this->width() + m_borderOffset, y());
			}
			else if (Right == m_enDriection)
			{
				this->resize(parent->width() *0.5, parent->height() - 34);
				m_nParentWidth = parent->width();
				move(m_nParentWidth - m_borderOffset, y());
			}		
		}		
	}	
	return QWidget::eventFilter(watched, event);
}

void AutoHide::leaveEvent(QEvent *event)
{
    if(m_isAutoHide)
    {
        hideWidget();
    }
    QWidget::leaveEvent(event);
}

void AutoHide::enterEvent(QEvent *event)
{
    if(m_isAutoHide)
    {
        showWidget();
    }
    QWidget::enterEvent(event);
}

void AutoHide::showWidget()
{
    QPoint pos = this->pos();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
	animation->setDuration(m_nAnimaDuration);
    animation->setStartValue(QRect(pos, this->size()));

    QRect rcEnd;
    if (m_enDriection & Up)
    {
        rcEnd = QRect(this->x(), 0, this->size().width(), this->rect().height());
    }
    else if (m_enDriection & Left)
    {
        rcEnd = QRect(0, this->y(), this->size().width(), this->rect().height());
    }
    else if (m_enDriection & Right)
    {
        rcEnd = QRect(m_nParentWidth - this->width(), this->y(), this->size().width(), this->rect().height());
    }
    animation->setEndValue(rcEnd);
    animation->start();
}

void AutoHide::hideWidget()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
	animation->setDuration(m_nAnimaDuration);
    animation->setStartValue(QRect(this->pos(), this->size()));

    QRect rcEnd;
    if (m_enDriection & Up)
    {
		rcEnd = QRect(m_nParentWidth / 10, -this->height() + m_borderOffset, this->size().width(), this->rect().height());
    }
    else if (m_enDriection & Left)
    {
		rcEnd = QRect(-this->width() + m_borderOffset, this->y(), this->size().width(), this->rect().height());
    }
    else if (m_enDriection & Right)
    {
		rcEnd = QRect(m_nParentWidth - m_borderOffset, this->y(), this->size().width(), this->rect().height());
    }
    animation->setEndValue(rcEnd);
    animation->start();
}

void AutoHide::displayHistory()
{
	QSettings settings(AUTOHIDE_BASEREG, QSettings::NativeFormat);
	settings.beginGroup(AUTOHIDE_GROUP);
	QStringList keys = settings.childKeys();
	for each (const QString& var in keys)
	{
		if (var == AUTOHIDE_DOCUMENTCOUNT)
			continue;
		QString strItem = settings.value(var).toString();
		QListWidgetItem* item = new QListWidgetItem(strItem);
		item->setSizeHint(QSize(this->width(), 100));				
		listWidget->addItem(item);
	}
	settings.endGroup();
}

void AutoHide::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case  Qt::Key_Delete:
		removeSelectItems();
		break;
	default:
		break;
	}
}
void AutoHide::removeSelectItems()
{
	QList<QListWidgetItem*>& indexList = listWidget->selectedItems();
	QListWidgetItem* pItem = NULL;
	for (int i = 0; i < indexList.size(); ++i)
	{
		pItem = indexList.at(i);
		if (pItem)
		{
			deleteHistory(pItem->text());
			listWidget->takeItem(listWidget->row(pItem));
		}		
	}
	listWidget->setCurrentRow(listWidget->count() - 1);
}

void AutoHide::deleteHistory(QString value)
{
	QSettings settings(AUTOHIDE_BASEREG, QSettings::NativeFormat);
	settings.beginGroup(AUTOHIDE_GROUP);
	QStringList keys = settings.childKeys();

	for (int i = 0; i < keys.size();++i)
	{
		QString record = settings.value(keys.at(i)).toString();
		if (record == value)
		{
			settings.remove(keys.at(i));
		}
	}
	settings.endGroup();
}

void AutoHide::initUi()
{
	this->resize(400, 588);
	verticalLayout = new QVBoxLayout(this);
	listWidget = new QListWidget(this);
	//多选
	listWidget->setSelectionMode(QListWidget::ExtendedSelection);	
	verticalLayout->addWidget(listWidget);
	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	horizontalSpacerLeft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout->addItem(horizontalSpacerLeft);
	checkBox = new QCheckBox(this);
	checkBox->setText(QString::fromLocal8Bit("固定"));
	horizontalLayout->addWidget(checkBox);
	horizontalSpacerRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayout->addItem(horizontalSpacerRight);
	verticalLayout->addLayout(horizontalLayout);

	connect(listWidget, &QListWidget::itemClicked, [=](QListWidgetItem *item){
		sig_ItemDoubleClicked(item->text());
		if (m_isAutoHide)
		{
			hideWidget();
		}		
	});
	connect(checkBox, &QCheckBox::stateChanged, [=](int state){
		m_isAutoHide = !state;
		sig_fixed(state);
	});
}
