/**
 * 自定义无边框窗体、对话框和提示框并封装成库
 *
 * framelesswindow.cpp
 * 实现了FramelessWindow、FramlessDialog、FramelessMessageBox
 *
 */

#include "framelesswindow.h"
#include "framelesshelper.h"
#include "titlebar.h"
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QApplication>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QEvent>
#include <QMessageBox>
#include <QIcon>
#include <QFile>
#include <qdrawutil.h>
#include <QBitmap>
#include <QDebug>
#include <QDesktopWidget>

FramelessDialog::FramelessDialog(QWidget *parent)
    : WidgetShadow<QDialog>(parent)
{

    resize(400, 300);
    setObjectName("framelessDialog");
    setWindowTitle("FramelessDialog");
    // 设置最大和还原按钮不可见
    setMinimumVisible(false);
    setMaximumVisible(false);
}

FramelessMessageBox::FramelessMessageBox(QWidget *parent, const QString &title, const QString &text,
                       QMessageBox::StandardButtons buttons,
                       QMessageBox::StandardButton defaultButton)
    : FramelessDialog(parent)
{
    setObjectName("framelessMessagBox");
    setMinimumVisible(false);
    setMaximumVisible(false);
    setWidgetResizable(false);
    setWindowTitle(title);
    setMinimumSize(300, 150);

    m_pButtonBox = new QDialogButtonBox(this);
    m_pButtonBox->setStandardButtons(QDialogButtonBox::StandardButtons(int(buttons)));
    setDefaultButton(defaultButton);

    QPushButton *pOkButton = m_pButtonBox->button(QDialogButtonBox::Ok);
    if(pOkButton != Q_NULLPTR){
        pOkButton->setObjectName("pOkButton");
    }

    QPushButton *pYesButton = m_pButtonBox->button(QDialogButtonBox::Yes);
    if(pYesButton != Q_NULLPTR){
        pYesButton->setObjectName("pYesButton");
    }

    QPushButton *pNoButton = m_pButtonBox->button(QDialogButtonBox::No);
    if(pNoButton != Q_NULLPTR){
        pNoButton->setObjectName("pNoButton");
    }

    QPushButton *pCloseButton = m_pButtonBox->button(QDialogButtonBox::Close);
    if(pCloseButton != Q_NULLPTR){
        pCloseButton->setObjectName("pCloseButton");
    }

    QPushButton *pCancelButton = m_pButtonBox->button(QDialogButtonBox::Cancel);
    if(pCancelButton != Q_NULLPTR){
        pCancelButton->setObjectName("pCancelButton");
    }

    m_pIconLabel = new QLabel(this);
    m_pLabel = new QLabel(this);

    QPixmap pixmap(":/images/msgbox/msg-info-48x48.png");
    m_pIconLabel->setPixmap(pixmap);
    m_pIconLabel->setFixedSize(35, 35);
    m_pIconLabel->setScaledContents(true);
    m_pIconLabel->setObjectName("m_pIconLabel");

    m_pLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pLabel->setObjectName("messageTextLabel");
    m_pLabel->setOpenExternalLinks(true);
    m_pLabel->setText(text);

    m_pGridLayout = new QGridLayout();
    m_pGridLayout->addWidget(m_pIconLabel, 0, 0, 2, 1, Qt::AlignTop);
    m_pGridLayout->addWidget(m_pLabel, 0, 1, 2, 1);
    m_pGridLayout->addWidget(m_pButtonBox, m_pGridLayout->rowCount(), 0, 1, m_pGridLayout->columnCount());
    m_pGridLayout->setSizeConstraint(QLayout::SetNoConstraint);
    m_pGridLayout->setHorizontalSpacing(10);
    m_pGridLayout->setVerticalSpacing(10);
    m_pGridLayout->setContentsMargins(10, 10, 10, 10);
    m_pFrameLessWindowLayout->addLayout(m_pGridLayout);

    translateUI();

    connect(m_pButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));

    // 计算文字宽度
    QFont wordfont;
    wordfont.setFamily(this->font().defaultFamily());
    wordfont.setPointSize(this->font().pointSize());
    QFontMetrics fm(wordfont);
    QRect rect = fm.boundingRect(text);

    resize(rect.width() + 60, 130);
}

FramelessMessageBox::~FramelessMessageBox()
{

}

QAbstractButton *FramelessMessageBox::clickedButton() const
{
    return m_pClickedButton;
}

QMessageBox::StandardButton FramelessMessageBox::standardButton(QAbstractButton *button) const
{
    return (QMessageBox::StandardButton)m_pButtonBox->standardButton(button);
}

void FramelessMessageBox::setDefaultButton(QPushButton *button)
{
    if (!m_pButtonBox->buttons().contains(button))
        return;

    m_pDefaultButton = button;
    button->setDefault(true);
    button->setFocus();
}

void FramelessMessageBox::setDefaultButton(QMessageBox::StandardButton button)
{
    setDefaultButton(m_pButtonBox->button(QDialogButtonBox::StandardButton(button)));
}

void FramelessMessageBox::setTitle(const QString &title)
{
    setWindowTitle(title);
}

void FramelessMessageBox::setText(const QString &text)
{
    m_pLabel->setText(text);
}

void FramelessMessageBox::setIcon(const QString &icon)
{
    m_pIconLabel->setPixmap(QPixmap(icon));
}

void FramelessMessageBox::hideInfoIcon()
{
    //隐藏Icon
    m_pIconLabel->setVisible(false);
    //重新设置文字位置
    m_pGridLayout->addWidget(m_pLabel, 0, 0, 2, 1, Qt::AlignTop);
}

void FramelessMessageBox::addWidget(QWidget *pWidget)
{
    m_pLabel->hide();
    m_pGridLayout->addWidget(pWidget, 0, 1, 2, 1);
}

QMessageBox::StandardButton FramelessMessageBox::showMessageBox(QWidget *parent,
                                                                const QString &title,
                                                                const QString &text,
                                                                QMessageBox::StandardButtons buttons,
                                                                IconType messageType,
                                                                QMessageBox::StandardButton defaultButton)
{
    FramelessMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.hideTitleBarIcon();

    switch (messageType) {
    case MSG_NOICON:
        msgBox.hideInfoIcon();
        break;
    case MSG_INFORMATION:
        msgBox.setIcon(":/images/msgbox/msg-info-48x48.png");
        break;
    case MSG_WARNNING:
        msgBox.setIcon(":/images/msgbox/msg-warning-48x48.png");
        break;
    case MSG_QUESTION:
        msgBox.setIcon(":/images/msgbox/msg-question-48x48.png");
        break;
    case MSG_ERROR:
        msgBox.setIcon(":/images/msgbox/msg-error-48x48.png");
        break;
    case MSG_SUCCESS:
        msgBox.setIcon(":/images/msgbox/msg-success-48x48.png");
        break;
    default:
        break;
    }

    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QMessageBox::StandardButton FramelessMessageBox::showInformation(QWidget *parent,
                                                                 const QString &title,
                                                                 const QString &text,
                                                                 QMessageBox::StandardButtons buttons,
                                                                 QMessageBox::StandardButton defaultButton)
{
    FramelessMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/images/msgbox/msg-info-48x48.png");
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QMessageBox::StandardButton FramelessMessageBox::showError(QWidget *parent,
                                                           const QString &title,
                                                           const QString &text,
                                                           QMessageBox::StandardButtons buttons,
                                                           QMessageBox::StandardButton defaultButton)
{
    FramelessMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/images/msgbox/msg-error-48x48.png");
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QMessageBox::StandardButton FramelessMessageBox::showSuccess(QWidget *parent,
                                                             const QString &title,
                                                             const QString &text,
                                                             QMessageBox::StandardButtons buttons,
                                                             QMessageBox::StandardButton defaultButton)
{
    FramelessMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/images/msgbox/msg-success-48x48.png");
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QMessageBox::StandardButton FramelessMessageBox::showQuestion(QWidget *parent,
                                                              const QString &title,
                                                              const QString &text,
                                                              QMessageBox::StandardButtons buttons,
                                                              QMessageBox::StandardButton defaultButton)
{
    FramelessMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/images/msgbox/msg-question-48x48.png");
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QMessageBox::StandardButton FramelessMessageBox::showWarning(QWidget *parent,
                                                             const QString &title,
                                                             const QString &text,
                                                             QMessageBox::StandardButtons buttons,
                                                             QMessageBox::StandardButton defaultButton)
{
    FramelessMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/images/msgbox/msg-warning-48x48.png");
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QMessageBox::StandardButton FramelessMessageBox::showCritical(QWidget *parent,
                                                              const QString &title,
                                                              const QString &text,
                                                              QMessageBox::StandardButtons buttons,
                                                              QMessageBox::StandardButton defaultButton)
{
    FramelessMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/images/msgbox/msg-warning-48x48.png");
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

QMessageBox::StandardButton FramelessMessageBox::showCheckBoxQuestion(QWidget *parent,
                                                                      const QString &title,
                                                                      const QString &text,
                                                                      QMessageBox::StandardButtons buttons,
                                                                      QMessageBox::StandardButton defaultButton)
{
    FramelessMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/images/msgbox/msg-question-48x48.png");

    QCheckBox *pCheckBox = new QCheckBox(&msgBox);
    pCheckBox->setText(text);
    msgBox.addWidget(pCheckBox);
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;

    QMessageBox::StandardButton standardButton = msgBox.standardButton(msgBox.clickedButton());
    if (standardButton == QMessageBox::Yes)
    {
        return pCheckBox->isChecked() ? QMessageBox::Yes : QMessageBox::No;
    }
    return QMessageBox::Cancel;
}

void FramelessMessageBox::changeEvent(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::LanguageChange:
        translateUI();
        break;

    default:
        FramelessDialog::changeEvent(event);
    }
}

void FramelessMessageBox::onButtonClicked(QAbstractButton *button)
{
    m_pClickedButton = button;
    done(execReturnCode(button));
}

void FramelessMessageBox::translateUI()
{
    QPushButton *pYesButton = m_pButtonBox->button(QDialogButtonBox::Yes);
    if (pYesButton != NULL)
        pYesButton->setText(tr("Yes"));

    QPushButton *pNoButton = m_pButtonBox->button(QDialogButtonBox::No);
    if (pNoButton != NULL)
        pNoButton->setText(tr("No"));

    QPushButton *pOkButton = m_pButtonBox->button(QDialogButtonBox::Ok);
    if (pOkButton != NULL)
        pOkButton->setText(tr("Ok"));

    QPushButton *pCancelButton = m_pButtonBox->button(QDialogButtonBox::Cancel);
    if (pCancelButton != NULL)
        pCancelButton->setText(tr("Cancel"));
}

int FramelessMessageBox::execReturnCode(QAbstractButton *button)
{
    int nResult = m_pButtonBox->standardButton(button);
    return nResult;
}
