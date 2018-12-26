/**
 * 自定义无边框窗体、对话框和提示框并封装成库
 *
 * titlebar.cpp
 * 自定义窗体的标题栏。
 *
 */

#include "titlebar.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <qt_windows.h>
#include <QDesktopWidget>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
    , m_bMaximizeDisabled(false)
{
    setFixedHeight(30);

    m_pIconLabel = new QLabel(this);
    m_pTitleLabel = new QLabel(this);
    m_pMinimizeButton = new StateButton(this);
    m_pMinimizeButton->setPixmap(QPixmap(":/images/titlebar/min.png"));
    m_pMaximizeButton = new StateButton(this);
    m_pMaximizeButton->setPixmap(QPixmap(":/images/titlebar/max.png"));
    m_pCloseButton = new StateButton(this);
    m_pCloseButton->setPixmap(QPixmap(":/images/titlebar/close.png"));

    m_pIconLabel->setFixedSize(20, 20);
    m_pIconLabel->setScaledContents(true);

    m_pTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_pTitleLabel->setObjectName("titleLabel");
    m_pMinimizeButton->setObjectName("minimizeButton");
    m_pMaximizeButton->setObjectName("maximizeButton");
    m_pCloseButton->setObjectName("closeButton");

    m_pMinimizeButton->setToolTip("Minimize");
    m_pMaximizeButton->setToolTip("Maximize");
    m_pCloseButton->setToolTip("Close");

    m_pMainLayout = new QHBoxLayout(this);
    m_pMainLayout->addSpacing(5);
    m_pMainLayout->addWidget(m_pIconLabel);
    m_pMainLayout->addSpacing(5);
    m_pMainLayout->addWidget(m_pTitleLabel);
    m_pMainLayout->addStretch();
    m_pMainLayout->addWidget(m_pMinimizeButton);
    m_pMainLayout->addWidget(m_pMaximizeButton);
    m_pMainLayout->addWidget(m_pCloseButton);
    m_pMainLayout->setSpacing(0);
    m_pMainLayout->setMargin(0);

    setLayout(m_pMainLayout);

    connect(m_pMinimizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pMaximizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pCloseButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
}
TitleBar::~TitleBar()
{

}

void TitleBar::setMinimumVisible(bool minimum)
{
    if (!minimum)  m_pMinimizeButton->hide();
}

void TitleBar::setMaximumVisible(bool maximum)
{
    if (!maximum) m_pMaximizeButton->hide();
}

void TitleBar::setMaximizeDisabled()
{
    m_bMaximizeDisabled = true;
}

void TitleBar::hideTitleIcon()
{
    m_pIconLabel->setVisible(false);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(m_bMaximizeDisabled) {
        return;
    }

    emit m_pMaximizeButton->clicked();
}

//void TitleBar::mousePressEvent(QMouseEvent *event)
//{
//    if (ReleaseCapture())
//    {
//        QWidget *pWindow = this->window();
//        if (pWindow->isTopLevel())
//        {
//           SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
//        }
//    }
//    event->ignore();
//}

bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::WindowTitleChange:
    {
        QWidget *pWidget = qobject_cast<QWidget *>(obj);
        if (pWidget)
        {
            m_pTitleLabel->setText(pWidget->windowTitle());
            return true;
        }
    }
    case QEvent::WindowIconChange:
    {
        QWidget *pWidget = qobject_cast<QWidget *>(obj);
        if (pWidget)
        {
            QIcon icon = pWidget->windowIcon();
            m_pIconLabel->setPixmap(icon.pixmap(m_pIconLabel->size()));
            return true;
        }
    }
    case QEvent::Move:
    case QEvent::WindowStateChange:
    case QEvent::Resize:
        updateMaximize();
        return true;
    default:
        return QWidget::eventFilter(obj, event);
    }
}

void TitleBar::onClicked()
{
    QPushButton *pButton = qobject_cast<QPushButton *>(sender());
    QWidget *pWindow = this->window();
    if (pWindow->isTopLevel())
    {
        if (pButton == m_pMinimizeButton)
        {
            pWindow->showMinimized();
        }
        else if (pButton == m_pMaximizeButton)
        {
            if(pWindow->isMaximized())
            {
                pWindow->showNormal();
                m_pMaximizeButton->setPixmap(QPixmap(":/images/titlebar/max.png"));
            }
            else
            {
                pWindow->showMaximized();
                window()->setGeometry(QApplication::desktop()->availableGeometry());
                m_pMaximizeButton->setPixmap(QPixmap(":/images/titlebar/restore.png"));
            }
        }
        else if (pButton == m_pCloseButton)
        {
            pWindow->close();
        }
    }
}

void TitleBar::updateMaximize()
{
    QWidget *pWindow = this->window();
    if (pWindow->isTopLevel())
    {
        bool bMaximize = pWindow->isMaximized();
        if (bMaximize)
        {
            m_pMaximizeButton->setToolTip(tr("Restore"));
            m_pMaximizeButton->setProperty("maximizeProperty", "restore");
        }
        else
        {
            m_pMaximizeButton->setProperty("maximizeProperty", "maximize");
            m_pMaximizeButton->setToolTip(tr("Maximize"));
        }

        m_pMaximizeButton->setStyle(QApplication::style());
    }
}
