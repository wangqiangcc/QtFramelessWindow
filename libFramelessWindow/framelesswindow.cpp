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

FramelessWindow::FramelessWindow(QWidget *parent)
    : QWidget(parent)
    , m_pHelper(Q_NULLPTR)
    , m_pTitleBar(Q_NULLPTR)
    , m_pMainWindow(Q_NULLPTR)
    , m_pMainLayout(Q_NULLPTR)
    , m_pFrameLessWindowLayout(Q_NULLPTR)
    , m_pCentralWdiget(new QWidget(this))
    , m_clientDrawType(kTopLeftToBottomRight)
    , m_redrawPixmap(true)
    , m_drawedPixmap(Q_NULLPTR)
{

    //设置默认背景色为白色
    setClientColor(QColor(255, 255, 255));
    //设置默认阴影边框
    m_borderImage.load(":/images/client-shadow.png", "8 8 8 8", "8 8 8 8");

    m_pMainWindow = new QWidget(this);
    m_pMainWindow->setObjectName("framelessWindow");
    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->addWidget(m_pMainWindow);
    m_pMainLayout->setContentsMargins(m_borderImage.margin());

    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    setAttribute(Qt::WA_TranslucentBackground);

    m_pTitleBar = new TitleBar(this);
    installEventFilter(m_pTitleBar);//标题栏不注册事件，注册本窗口把事件转发到标题栏
    setTitleHeight(m_pTitleBar->height());

    resize(800, 600);
    setWindowTitle("Custom Window");
    setWindowIcon(QIcon(":/images/logo.jpg"));

    m_pFrameLessWindowLayout = new QVBoxLayout(m_pMainWindow);
    m_pFrameLessWindowLayout->addWidget(m_pTitleBar);
    m_pFrameLessWindowLayout->addWidget(m_pCentralWdiget, 1);
    m_pFrameLessWindowLayout->setSpacing(0);
    m_pFrameLessWindowLayout->setContentsMargins(0, 0, 0, 0);

    m_pHelper = new FramelessHelper(this);
    m_pHelper->activateOn(this);  //激活当前窗体

    //设置边框宽度
    m_pHelper->setBorderWidth((m_borderImage.margin().top()+m_borderImage.margin().left()+m_borderImage.margin().right()+m_borderImage.margin().bottom())/4);

    setWidgetMovalbe();
    setWidgetResizable();
    setRubberBandOnMove(false);
    setRubberBandOnResize(false);
}

FramelessWindow::~FramelessWindow()
{
    qDeleteAll(m_alphaCache);
    m_alphaCache.clear();

    if(m_drawedPixmap) {
        delete m_drawedPixmap;
        m_drawedPixmap = NULL;
    }
}

void FramelessWindow::setStyleSheetFile(const QString &file)
{
    QFile qss(file);
    qss.open(QFile::ReadOnly);
    this->setStyleSheet(qss.readAll());
    qss.close();
}

void FramelessWindow::setTitleHeight(int h)
{
    m_pHelper->setTitleHeight(h);
}

void FramelessWindow::setWidgetMovalbe(bool movable)
{
    m_pHelper->setWidgetMovable(movable);
}

void FramelessWindow::setWidgetResizable(bool resizable)
{
    m_pHelper->setWidgetResizable(resizable);
}

void FramelessWindow::setMinimumVisible(bool vislble)
{
    m_pTitleBar->setMinimumVisible(vislble);
}

void FramelessWindow::setMaximumVisible(bool visible)
{
    m_pTitleBar->setMaximumVisible(visible);
}

void FramelessWindow::setRubberBandOnMove(bool move)
{
    m_pHelper->setRubberBandOnMove(move);
}

void FramelessWindow::setRubberBandOnResize(bool resize)
{
    m_pHelper->setRubberBandOnResize(resize);
}

void FramelessWindow::setCentralWidget(QWidget *w)
{
    m_pCentralWdiget->deleteLater();
    m_pCentralWdiget = w;
    m_pFrameLessWindowLayout->addWidget(w, 1);
}

void FramelessWindow::setBorderImage(const QString &image, const QMargins& border, const QMargins& margin)
{
    m_borderImage.setPixmap(image);
    m_borderImage.setBorder(border);
    m_borderImage.setMargin(margin);
    update();
}

void FramelessWindow::setClientImage(const QString &image)
{
    m_clientPixmap.load(image);
    update();
}

void FramelessWindow::setClientColor(const QColor &color)
{
    //画一个1*1大小的纯色图片
    QPixmap pixmap(1,1);       //作为绘图设备
    QPainter painter(&pixmap); //创建一直画笔
    painter.fillRect(0,0,1,1, color);
    m_clientPixmap = pixmap;

    update();
}

void FramelessWindow::hideTitleBar()
{
    m_pFrameLessWindowLayout->removeWidget(m_pTitleBar);
    m_pTitleBar->deleteLater();
    m_pTitleBar = Q_NULLPTR;
}

QRect FramelessWindow::clientRect() const
{
    QRect clientRect(rect());
    if(!isMaximized()) {
        const QMargins& m = m_borderImage.margin();
        clientRect.adjust(m.left(), m.top(), -m.right(), -m.bottom());
    }

    return clientRect;
}

void FramelessWindow::drawTopLeft(QPainter *painter, const QRect &rect, const QPixmap &pixmap)
{
    //图片比要画的区域大，剪辑
    int width = qMin(pixmap.width(), rect.width());
    int height = qMin(pixmap.height(), rect.height());

    painter->drawPixmap(rect.left(), rect.top(), pixmap, 0, 0, width, height);

    //图片宽度比要画的区域小，拉伸宽度
    if(width < rect.width()) {
        QRect dst(rect.left() + width, rect.top(), rect.width() - width, height/*r.height()*/);
        QRect src(width - 1, 0, 1, height);
        painter->drawPixmap(dst, pixmap, src);
    }

    //图片高度比要画的区域小，拉伸高度
    if(height < rect.height()) {
        QRect dst(rect.left(), rect.top() + height, width/*r.width()*/, rect.height() - height);
        QRect src(0, height - 1, width, 1);
        painter->drawPixmap(dst, pixmap, src);
    }

    //图片高度、宽度都比要画的区域小，拉伸最右下角
    if(width < rect.width() && height < rect.height()) {
        QRect dst(rect.left() + width, rect.top() + height, rect.width() - width, rect.height() - height);
        QRect src(width - 1, height - 1, 1, 1);  //用最后一个点拉伸
        painter->drawPixmap(dst, pixmap, src);
    }
}

void FramelessWindow::drawTopRight(QPainter *painter, const QRect &rect, const QPixmap &pixmap)
{
    int width = qMin(pixmap.width(), rect.width());
    int height = qMin(rect.height(), pixmap.height());

    //左上, 图的左边往左拉伸
    if(rect.width() > pixmap.width()) {
        QRect src(0, 0, 1, height);

        QRect dst(rect.left(), rect.top(), rect.width() - pixmap.width(), height);
        painter->drawPixmap(dst, pixmap, src);
    }

    //右上()
    {
        QRect src(pixmap.width() - width, 0, width, height);
        QRect dst(rect.left() + rect.width() - width, rect.top(), width, height);
        painter->drawPixmap(dst, pixmap, src);
    }

    //左下(拉伸)
    if(rect.width() > pixmap.width() && rect.height() > pixmap.height()) {
        QRect src(0, height - 1, 1, 1);
        QRect dst(0, height, rect.width() - pixmap.width(), rect.height() - pixmap.height());
        painter->drawPixmap(dst, pixmap, src);
    }

    //右下(拉伸), 图的下边往下拉伸
    if(rect.height() > pixmap.height()) {
        QRect src(pixmap.width() - width, height - 1, width, 1);
        QRect dst(rect.left() + rect.width() - width, height, width, rect.height() - pixmap.height());
        painter->drawPixmap(dst, pixmap, src);
    }
}

void FramelessWindow::resizeEvent(QResizeEvent *event)
{
    if(event->size() == event->oldSize()) {
        return;
    }

    m_redrawPixmap = true;

    //判断是否最大化
    //这里没有使用 isMaximized因为有时候不准确
    if(QApplication::desktop()->availableGeometry().width() == geometry().width() && \
       QApplication::desktop()->availableGeometry().height() == geometry().height()) {
        //无圆角,并禁止改变窗口大小
        this->clearMask();
        //最大化后，无边框无边距
        m_pMainLayout->setContentsMargins(0, 0, 0, 0);
        return;

    } else {
        //恢复窗口的边框边距
        if(m_pMainLayout->contentsMargins().isNull()) {
            m_pMainLayout->setContentsMargins(m_borderImage.margin());
        }
    }

#if 1
    //圆角窗口
    QBitmap  pixmap(event->size());  //生成一张位图
    QPainter painter(&pixmap);  //QPainter用于在位图上绘画
    // 圆角平滑
    painter.setRenderHints(QPainter::Antialiasing, true);

    //填充位图矩形框(用白色填充)
    QRect r = this->rect();
    painter.fillRect(r, Qt::color0);
    painter.setBrush(Qt::color1);
    //在位图上画圆角矩形(用黑色填充)
    painter.drawRoundedRect(r, 0, 0);
    painter.end();

    //使用setmask过滤即可
    setMask(pixmap);
#endif
}

void FramelessWindow::paintEvent(QPaintEvent *event)
{    
    if(m_redrawPixmap || !m_drawedPixmap) {
        m_redrawPixmap = false;
        qDeleteAll(m_alphaCache);
        m_alphaCache.clear();

        QRect rect = this->rect();

        delete m_drawedPixmap; //it's safe to delete null
        m_drawedPixmap = new QPixmap(rect.width(), rect.height());
        m_drawedPixmap->fill(Qt::transparent);//Qt::black

        QPainter painter(m_drawedPixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);

        //边框背景图
        const QMargins &m  = m_borderImage.border();
        if(isMaximized()) {  //最大化后，无边框
            //考虑只有一个borderimage作为背景的情况，这种情况最大化后就需要用borderimage去掉边框后作为背景图
            //把rect放大正好使边框看不见.
            rect.adjust(-m.left(), -m.top(), m.right(), m.bottom());
        }
        qDrawBorderPixmap(&painter, rect, m, m_borderImage.pixmap());

        //客户区图
        const QPixmap& bmp = m_clientPixmap;
        if(!bmp.isNull()) {

            rect = clientRect();

            QPixmap pixmap(rect.size());
            {
                QPainter p(&pixmap);
                //1-从左上固定，右下拉伸；2-右上固定，左下拉伸
                switch(clientDrawType()) {
                case kTopLeftToBottomRight:
                    drawTopLeft(&p, pixmap.rect(), bmp);
                    break;
                case kTopRightToBottomLeft:
                    drawTopRight(&p, pixmap.rect(), bmp);
                    break;
                default:
                    p.drawPixmap(pixmap.rect(), bmp);
                    break;
                }
            }

            painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);//CompositionMode_DestinationAtop,CompositionMode_SoftLight,CompositionMode_Multiply
            painter.drawPixmap(rect.left(), rect.top(), pixmap);
        }
    }

    QPainter painter(this);
    painter.drawPixmap(0, 0, *m_drawedPixmap);
}

//bool FramelessWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
//{
//    MSG* msg = (MSG *)message;
//    switch (msg->message)
//    {
//    case WM_NCCALCSIZE:
//    {
//        // this kills the window frame and title bar we added with WS_THICKFRAME and WS_CAPTION
//        *result = 0;
//        return true;
//    }
//    default:
//        return QWidget::nativeEvent(eventType, message, result);
//    }
//}

FramelessDialog::FramelessDialog(QWidget *parent)
    : QDialog(parent)
    , m_pHelper(Q_NULLPTR)
    , m_pTitleBar(Q_NULLPTR)
    , m_pFrameLessWindowLayout(Q_NULLPTR)
    , m_pCentralWidget(new QWidget(this))
    , m_clientDrawType(kTopLeftToBottomRight)
    , m_redrawPixmap(true)
    , m_drawedPixmap(Q_NULLPTR)
{

    setObjectName("framelessDialog");
    resize(400, 300);

    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    setAttribute(Qt::WA_TranslucentBackground);

    m_pTitleBar = new TitleBar(this);
    m_pTitleBar->setMaximizeDisabled();
    installEventFilter(m_pTitleBar);
    setTitleHeight(m_pTitleBar->height());

    setWindowTitle("Custom Window");

    //设置默认背景色为白色
    setClientColor(QColor(255, 255, 255));
    //设置默认阴影边框
    m_borderImage.load(":/images/background/client-shadow.png", "8 8 8 8", "8 8 8 8");

    m_pFrameLessWindowLayout = new QVBoxLayout();
    m_pFrameLessWindowLayout->addWidget(m_pTitleBar);
    m_pFrameLessWindowLayout->addWidget(m_pCentralWidget, 1);
    m_pFrameLessWindowLayout->setSpacing(0);
    m_pFrameLessWindowLayout->setContentsMargins(m_borderImage.margin());

    setLayout(m_pFrameLessWindowLayout);

    m_pHelper = new FramelessHelper(this);
    m_pHelper->activateOn(this);  //激活当前窗体

    //设置边框宽度
    m_pHelper->setBorderWidth((m_borderImage.margin().top()+m_borderImage.margin().left()+m_borderImage.margin().right()+m_borderImage.margin().bottom())/4);

    setWidgetMovalbe();
    setWidgetResizable(false);
    setRubberBandOnMove(false);
    setRubberBandOnResize(false);
    // 设置最大和还原按钮不可见
    setMinimumVisible(false);
    setMaximumVisible(false);
}

void FramelessDialog::setStyleSheetFile(const QString &file)
{
    QFile qss(file);
    qss.open(QFile::ReadOnly);
    this->setStyleSheet(qss.readAll());
    qss.close();
}

void FramelessDialog::setTitleHeight(int h)
{
    m_pHelper->setTitleHeight(h);
}

void FramelessDialog::setWidgetMovalbe(bool movable)
{
    m_pHelper->setWidgetMovable(movable);
}

void FramelessDialog::setWidgetResizable(bool resizable)
{
    m_pHelper->setWidgetResizable(resizable);
}

void FramelessDialog::setMinimumVisible(bool vislble)
{
    m_pTitleBar->setMinimumVisible(vislble);
}

void FramelessDialog::setMaximumVisible(bool visible)
{
    m_pTitleBar->setMaximumVisible(visible);
}

void FramelessDialog::setRubberBandOnMove(bool move)
{
    m_pHelper->setRubberBandOnMove(move);
}

void FramelessDialog::setRubberBandOnResize(bool resize)
{
    m_pHelper->setRubberBandOnResize(resize);
}

void FramelessDialog::setCentralWidget(QWidget *w)
{
    m_pCentralWidget->deleteLater();
    m_pCentralWidget = w;
    m_pFrameLessWindowLayout->addWidget(m_pCentralWidget, 1);
}

void FramelessDialog::setBorderImage(const QString &image, const QMargins& border, const QMargins& margin)
{
    m_borderImage.setPixmap(image);
    m_borderImage.setBorder(border);
    m_borderImage.setMargin(margin);
    update();
}

void FramelessDialog::setClientImage(const QString &image)
{
    m_clientPixmap.load(image);
    update();
}

void FramelessDialog::setClientColor(const QColor &color)
{
    //画一个1*1大小的纯色图片
    QPixmap pixmap(1,1);       //作为绘图设备
    QPainter painter(&pixmap); //创建一直画笔
    painter.fillRect(0,0,1,1, color);
    m_clientPixmap = pixmap;

    update();
}

QRect FramelessDialog::clientRect() const
{
    QRect clientRect(rect());
    if(!isMaximized()) {
        const QMargins& m = m_borderImage.margin();
        clientRect.adjust(m.left(), m.top(), -m.right(), -m.bottom());
    }

    return clientRect;
}

void FramelessDialog::drawTopLeft(QPainter *painter, const QRect &rect, const QPixmap &pixmap)
{
    //图片比要画的区域大，剪辑
    int width = qMin(pixmap.width(), rect.width());
    int height = qMin(pixmap.height(), rect.height());

    painter->drawPixmap(rect.left(), rect.top(), pixmap, 0, 0, width, height);

    //图片宽度比要画的区域小，拉伸宽度
    if(width < rect.width()) {
        QRect dst(rect.left() + width, rect.top(), rect.width() - width, height/*r.height()*/);
        QRect src(width - 1, 0, 1, height);
        painter->drawPixmap(dst, pixmap, src);
    }

    //图片高度比要画的区域小，拉伸高度
    if(height < rect.height()) {
        QRect dst(rect.left(), rect.top() + height, width/*r.width()*/, rect.height() - height);
        QRect src(0, height - 1, width, 1);
        painter->drawPixmap(dst, pixmap, src);
    }

    //图片高度、宽度都比要画的区域小，拉伸最右下角
    if(width < rect.width() && height < rect.height()) {
        QRect dst(rect.left() + width, rect.top() + height, rect.width() - width, rect.height() - height);
        QRect src(width - 1, height - 1, 1, 1);  //用最后一个点拉伸
        painter->drawPixmap(dst, pixmap, src);
    }
}

void FramelessDialog::drawTopRight(QPainter *painter, const QRect &rect, const QPixmap &pixmap)
{
    int width = qMin(pixmap.width(), rect.width());
    int height = qMin(rect.height(), pixmap.height());

    //左上, 图的左边往左拉伸
    if(rect.width() > pixmap.width()) {
        QRect src(0, 0, 1, height);

        QRect dst(rect.left(), rect.top(), rect.width() - pixmap.width(), height);
        painter->drawPixmap(dst, pixmap, src);
    }

    //右上()
    {
        QRect src(pixmap.width() - width, 0, width, height);
        QRect dst(rect.left() + rect.width() - width, rect.top(), width, height);
        painter->drawPixmap(dst, pixmap, src);
    }

    //左下(拉伸)
    if(rect.width() > pixmap.width() && rect.height() > pixmap.height()) {
        QRect src(0, height - 1, 1, 1);
        QRect dst(0, height, rect.width() - pixmap.width(), rect.height() - pixmap.height());
        painter->drawPixmap(dst, pixmap, src);
    }

    //右下(拉伸), 图的下边往下拉伸
    if(rect.height() > pixmap.height()) {
        QRect src(pixmap.width() - width, height - 1, width, 1);
        QRect dst(rect.left() + rect.width() - width, height, width, rect.height() - pixmap.height());
        painter->drawPixmap(dst, pixmap, src);
    }
}

void FramelessDialog::resizeEvent(QResizeEvent *event)
{
    if(event->size() == event->oldSize()) {
        return;
    }

    m_redrawPixmap = true;

    //判断是否最大化
    //这里没有使用 isMaximized因为有时候不准确
    if(QApplication::desktop()->availableGeometry().width() == geometry().width() && \
       QApplication::desktop()->availableGeometry().height() == geometry().height()) {
        //无圆角,并禁止改变窗口大小
        this->clearMask();
        //最大化后，无边框无边距
        m_pFrameLessWindowLayout->setContentsMargins(0, 0, 0, 0);
        return;

    } else {
        //恢复窗口的边框边距
        if(m_pFrameLessWindowLayout->contentsMargins().isNull()) {
            m_pFrameLessWindowLayout->setContentsMargins(m_borderImage.margin());
        }
    }

#if 1
    //圆角窗口
    QBitmap  pixmap(event->size());  //生成一张位图
    QPainter painter(&pixmap);  //QPainter用于在位图上绘画
    // 圆角平滑
    painter.setRenderHints(QPainter::Antialiasing, true);

    //填充位图矩形框(用白色填充)
    QRect r = this->rect();
    painter.fillRect(r, Qt::color0);
    painter.setBrush(Qt::color1);
    //在位图上画圆角矩形(用黑色填充)
    painter.drawRoundedRect(r, 0, 0);
    painter.end();

    //使用setmask过滤即可
    setMask(pixmap);
#endif
}

void FramelessDialog::paintEvent(QPaintEvent *event)
{
    if(m_redrawPixmap || !m_drawedPixmap) {
        m_redrawPixmap = false;
        qDeleteAll(m_alphaCache);
        m_alphaCache.clear();

        QRect rect = this->rect();

        delete m_drawedPixmap; //it's safe to delete null
        m_drawedPixmap = new QPixmap(rect.width(), rect.height());
        m_drawedPixmap->fill(Qt::transparent);//Qt::black

        QPainter painter(m_drawedPixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);

        //边框背景图
        const QMargins &m  = m_borderImage.border();
        if(isMaximized()) {  //最大化后，无边框
            //考虑只有一个borderimage作为背景的情况，这种情况最大化后就需要用borderimage去掉边框后作为背景图
            //把rect放大正好使边框看不见.
            rect.adjust(-m.left(), -m.top(), m.right(), m.bottom());
        }
        qDrawBorderPixmap(&painter, rect, m, m_borderImage.pixmap());

        //客户区图
        const QPixmap& bmp = m_clientPixmap;
        if(!bmp.isNull()) {

            rect = clientRect();

            QPixmap pixmap(rect.size());
            {
                QPainter p(&pixmap);
                //1-从左上固定，右下拉伸；2-右上固定，左下拉伸
                switch(clientDrawType()) {
                case kTopLeftToBottomRight:
                    drawTopLeft(&p, pixmap.rect(), bmp);
                    break;
                case kTopRightToBottomLeft:
                    drawTopRight(&p, pixmap.rect(), bmp);
                    break;
                default:
                    p.drawPixmap(pixmap.rect(), bmp);
                    break;
                }
            }

            painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);//CompositionMode_DestinationAtop,CompositionMode_SoftLight,CompositionMode_Multiply
            painter.drawPixmap(rect.left(), rect.top(), pixmap);
        }
    }

    QPainter painter(this);
    painter.drawPixmap(0, 0, *m_drawedPixmap);
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
    setMinimumSize(300, 130);
    resize(300, 130);

    m_pButtonBox = new QDialogButtonBox(this);
    m_pButtonBox->setStandardButtons(QDialogButtonBox::StandardButtons(int(buttons)));
    setDefaultButton(defaultButton);

    // 根据用到的按钮进行设置,太多了就不一一写了
    QPushButton *pYesButton = m_pButtonBox->button(QDialogButtonBox::Ok);
    if (pYesButton != NULL)
    {
        pYesButton->setObjectName("yesButton");
        pYesButton->setStyleSheet("QPushButton#yesButton { \
                                        background-color: #303030; \
                                        border: none; \
                                        width: 60px; \
                                        height: 25px; \
                                        color: white; \
                                   } \
                                   QPushButton#yesButton:hover { \
                                        background-color: #505050; \
                                   } ");
    }

    m_pIconLabel = new QLabel(this);
    m_pLabel = new QLabel(this);

    QPixmap pixmap(":/Images/information");
    m_pIconLabel->setPixmap(pixmap);
    m_pIconLabel->setFixedSize(35, 35);
    m_pIconLabel->setScaledContents(true);

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

void FramelessMessageBox::addWidget(QWidget *pWidget)
{
    m_pLabel->hide();
    m_pGridLayout->addWidget(pWidget, 0, 1, 2, 1);
}

QMessageBox::StandardButton FramelessMessageBox::showInformation(QWidget *parent,
                                                                 const QString &title,
                                                                 const QString &text,
                                                                 QMessageBox::StandardButtons buttons,
                                                                 QMessageBox::StandardButton defaultButton)
{
    FramelessMessageBox msgBox(parent, title, text, buttons, defaultButton);
    msgBox.setIcon(":/images/informationBlue_48.png");
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
    msgBox.setIcon(":/images/errorRed_48.png");
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
    msgBox.setIcon(":/images/successGreen_48.png");
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
    msgBox.setIcon(":/images/questionBlue_48.png");
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
    msgBox.setIcon(":/images/warningYellow_48.png");
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
    msgBox.setIcon(":/images/warningYellow_48.png");
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
    msgBox.setIcon(":/images/questionBlue_48.png");

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
