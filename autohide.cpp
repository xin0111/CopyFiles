#include "autohide.h"
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QPoint>
#include <QDebug>
#include <QSettings>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include "Tools.h"
#include "fileslistwidget.h"

#define  AUTOHIDE_BASEREG "HKEY_CURRENT_USER\\Software\\CopyFiles"
#define  AUTOHIDE_GROUP "Recent Document List"
#define  AUTOHIDE_DOCUMENT "Document"

class AutoHideUI 
{
public:
	AutoHideUI(){

	}
	void setupUi(QWidget *Form)
	{
		verticalLayout_2 = new QVBoxLayout(Form);
		verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
		verticalLayout_2->setContentsMargins(0, -1, 0, -1);
		horizontalLayout = new QHBoxLayout();
		horizontalLayout->setSpacing(0);
		horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
		listWidget = new FilesListWidget(Form);
		listWidget->setObjectName(QStringLiteral("listWidget"));

		horizontalLayout->addWidget(listWidget);

		verticalLayout = new QVBoxLayout();
		verticalLayout->setSpacing(0);
		verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
		verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

		verticalLayout->addItem(verticalSpacer);

		label_history = new QLabel(Form);

		verticalLayout->addWidget(label_history);

		verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

		verticalLayout->addItem(verticalSpacer_2);


		horizontalLayout->addLayout(verticalLayout);


		verticalLayout_2->addLayout(horizontalLayout);

		horizontalLayout_2 = new QHBoxLayout();
		horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
		horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout_2->addItem(horizontalSpacer);

		checkBox = new QCheckBox(Form);
		checkBox->setObjectName(QStringLiteral("checkBox"));

		horizontalLayout_2->addWidget(checkBox);

		horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout_2->addItem(horizontalSpacer_2);

		verticalLayout_2->addLayout(horizontalLayout_2);

		QMetaObject::connectSlotsByName(Form);
	} // setupUi
public:
	QVBoxLayout *verticalLayout_2;
	QHBoxLayout *horizontalLayout;
	FilesListWidget *listWidget;
	QVBoxLayout *verticalLayout;
	QSpacerItem *verticalSpacer;
	QLabel		*label_history;
	QSpacerItem *verticalSpacer_2;
	QHBoxLayout *horizontalLayout_2;
	QSpacerItem *horizontalSpacer;
	QCheckBox   *checkBox;
	QSpacerItem *horizontalSpacer_2;
};

AutoHide::AutoHide(QWidget *parent) :
QWidget(parent),
ui(new AutoHideUI)
{
	ui->setupUi(this);
	initUi();
    m_nParentWidth = parent->width();
	parent->installEventFilter(this);
	m_enDriection = Left;
	m_borderOffset = 32;
	m_nAnimaDuration = 300;
	m_isAutoHide = true;
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
	delete ui;
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
	int nNextIndex = CTools::CalcNextIndex(child.size(), child);
	settings.setValue(AUTOHIDE_DOCUMENT + QString::number(nNextIndex), filePath);
	settings.endGroup();
	//显示
	addListItem(filePath);
}

bool AutoHide::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == this->parent())
	{
		if (event->type() == QEvent::Resize)
		{
			QWidget* parent = qobject_cast<QWidget*>(watched);

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
				move(-this->width() + ui->label_history->width(), y());
			}
			else if (Right == m_enDriection)
			{
				this->resize(parent->width() *0.5, parent->height() - 34);
				m_nParentWidth = parent->width();
				move(m_nParentWidth - m_borderOffset, y());
			}		
		}		
	}	
	if (watched == ui->label_history)
	{
		if (event->type() == QEvent::Enter)
		{
			if (m_isAutoHide)
				showWidget();
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
		rcEnd = QRect(/*-this->width() + m_borderOffset*/-ui->listWidget->width(), this->y(), this->size().width(), this->rect().height());
    }
    else if (m_enDriection & Right)
    {
		rcEnd = QRect(m_nParentWidth - m_borderOffset, this->y(), this->size().width(), this->rect().height());
    }
    animation->setEndValue(rcEnd);
    animation->start();
}


void AutoHide::addListItem(QString filePath)
{
	QFontMetrics fm(this->font());
	QListWidgetItem* item = new QListWidgetItem(filePath);
	item->setSizeHint(QSize(qMax(this->width(), fm.width(filePath)), 100));
	item->setToolTip(filePath);
	ui->listWidget->addItem(item);
}

void AutoHide::displayHistory()
{
	QSettings settings(AUTOHIDE_BASEREG, QSettings::NativeFormat);
	settings.beginGroup(AUTOHIDE_GROUP);
	QStringList keys = settings.childKeys();
	for each (const QString& var in keys)
	{
		QString strItem = settings.value(var).toString();
		addListItem(strItem);
	}
	settings.endGroup();
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
	ui->listWidget->setDoubleEdit(false);
	ui->checkBox->setText(QString::fromLocal8Bit("固定"));
	
	QPixmap *pixmap = new QPixmap(":/images/history.png"); 
	pixmap->size();
	ui->label_history->resize(pixmap->size());
	ui->label_history->setPixmap(*pixmap);
	ui->label_history->setToolTip(QString::fromLocal8Bit("历史记录"));
	ui->label_history->installEventFilter(this);
	connect(ui->listWidget, &QListWidget::itemDoubleClicked, [=](QListWidgetItem *item){
		sig_ItemDoubleClicked(item->text());
		if (m_isAutoHide)
		{
			hideWidget();
		}
	});
	connect(ui->checkBox, &QCheckBox::stateChanged, [=](int state){
		m_isAutoHide = !state;
		ui->label_history->setVisible(m_isAutoHide);		
		sig_fixed(state);
	});
}
