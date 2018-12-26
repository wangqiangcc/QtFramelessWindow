/**
 * 自定义窗口阴影
 *
 * widgetshadow.h
 *
 */

#ifndef WIDGETSHADOW_H
#define WIDGETSHADOW_H

#include "framelesswindow_global.h"
#include "borderimage.h"
#include "framelesshelper.h"
#include "titlebar.h"
#include <QtWidgets>
#include <QDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QEventLoop>


template <class T>
class WidgetShadow : public T
{
public:
    typedef typename WidgetShadow<T> BaseClass;

    WidgetShadow(QWidget *parent = nullptr)
        :T(parent)
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

        resize(800, 600);
        setWindowTitle("FramelessWindow");

        //设置默认背景色为白色
        setClientColor(QColor(255, 255, 255));
        //设置默认阴影边框
        m_borderImage.load(":/images/background/client-shadow.png", "8 8 8 8", "8 8 8 8");

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

    ~WidgetShadow()
    {
        qDeleteAll(m_alphaCache);
        m_alphaCache.clear();

        if(m_drawedPixmap) {
            delete m_drawedPixmap;
            m_drawedPixmap = NULL;
        }
    }

    enum ClientDrawType {
        kTopLeftToBottomRight = 1,  //左下到右下
        kTopRightToBottomLeft       //右上到左下
    };

    /**
     * @brief setStyleSheetFile
     * @note 设置QSS样式文件
     * @param file
     */
    void setStyleSheetFile(const QString &file)
    {
        QFile qss(file);
        qss.open(QFile::ReadOnly);
        this->setStyleSheet(qss.readAll());
        qss.close();
    }

    /**
     * @brief setTitleHeight
     * @note 设置标题栏高度，如果设置了窗口可移动，拖动标题栏可以移动窗体
     * @param h
     * @note 标题栏的高度,默认是25
     */
    void setTitleHeight(int h = 25)
    {
        m_pHelper->setTitleHeight(h);
    }

    /**
     * @brief setWidgetMovalbe
     * @note 设置窗口是否可移动，默认可移动
     * @param movable
     */
    void setWidgetMovalbe(bool movable = true)
    {
        m_pHelper->setWidgetMovable(movable);
    }

    /**
     * @brief setWidgetResizable
     * @note 设置窗口是否可缩放，默认是可以进行缩放
     * @param resizable
     */
    void setWidgetResizable(bool resizable = true)
    {
        m_pHelper->setWidgetResizable(resizable);
    }

    /**
     * @brief setMinimumVisible
     * @note 设置窗口标题栏最小化按钮是否可见
     * @param vislble
     */
    void setMinimumVisible(bool vislble = true)
    {
        m_pTitleBar->setMinimumVisible(vislble);
    }

    /**
     * @brief setMaximumVisible
     * @note 设置窗口标题栏最大化或还原按钮是否可见
     * @param visible
     */
    void setMaximumVisible(bool visible = true)
    {
        m_pTitleBar->setMaximumVisible(visible);
    }

    /**
     * @brief setRubberBandOnMove
     * @note 设置窗口缩放时橡皮筋是否可移动，默认是可移动
     * @param rubber
     */
    void setRubberBandOnMove(bool move = true)
    {
        m_pHelper->setRubberBandOnMove(move);
    }

    /**
     * @brief setRubberBandOnResize
     * @note 设置窗口缩放时橡皮筋是否可缩放，默认可缩放
     * @param resize
     */
    void setRubberBandOnResize(bool resize = true)
    {
        m_pHelper->setRubberBandOnResize(resize);
    }

    /**
     * @brief setCentralWidget
     * @note 设置中心界面
     * @param w
     *  QWidget *
     */
    void setCentralWidget(QWidget *w)
    {
        m_pCentralWdiget->deleteLater();
        m_pCentralWdiget = w;
        m_pFrameLessWindowLayout->addWidget(w, 1);
    }


    /**
     * @brief setBorderImage
     * @param image
     * @note 设置边框图片或阴影
     */
    void setBorderImage(const QString &image, const QMargins& border = {8,8,8,8}, const QMargins& margin = {6,6,6,6})
    {
        m_borderImage.setPixmap(image);
        m_borderImage.setBorder(border);
        m_borderImage.setMargin(margin);
        update();
    }

    /**
     * @brief setClientImage
     * @param file
     * @note 设置窗口背景片
     */
    void setClientImage(const QString &image)
    {
        m_clientPixmap.load(image);
        update();
    }

    /**
     * @brief setClientColor
     * @param color
     * @note 设置背景颜色
     */
    void setClientColor(const QColor &color)
    {
        //画一个1*1大小的纯色图片
        QPixmap pixmap(1,1);       //作为绘图设备
        QPainter painter(&pixmap); //创建一直画笔
        painter.fillRect(0,0,1,1, color);
        m_clientPixmap = pixmap;

        update();
    }

    /**
     * @brief clientDrawType
     * @note 客户区背景绘制方式
     * @return
     */
    inline ClientDrawType clientDrawType() const
    {
        return m_clientDrawType;
    }
    void setClientDrawType(ClientDrawType type)
    {
        m_clientDrawType = type;
    }

    /**
     * @brief hideTitleBar
     * @note 不显示标题栏
     */
    void hideTitleBar()
    {
        m_pFrameLessWindowLayout->removeWidget(m_pTitleBar);
        m_pTitleBar->deleteLater();
        m_pTitleBar = Q_NULLPTR;
    }

    /**
     * @brief hideTitleBarIcon
     * @note 隐藏标题栏icon
     */
    void hideTitleBarIcon()
    {
        m_pTitleBar->hideTitleIcon();
    }

    /**
     * @brief 除去边框后的客户区rect
     * @return
     */
    QRect clientRect() const
    {
        QRect clientRect(rect());
        if(!isMaximized()) {
            const QMargins& m = m_borderImage.margin();
            clientRect.adjust(m.left(), m.top(), -m.right(), -m.bottom());
        }

        return clientRect;
    }

    /**
     * @brief 画背景图, 左上角画原始图，右下角拉伸
     * @param painter
     * @param rect
     * @param img
     */
    void drawTopLeft(QPainter *painter, const QRect& rect, const QPixmap& pixmap)
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

    /**
     * @brief 画背景图, 右上角画原始图，左下角拉伸
     * @param painter
     * @param rect
     * @param img
     */
    void drawTopRight(QPainter *painter, const QRect& rect, const QPixmap& pixmap)
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

protected:
    virtual void resizeEvent(QResizeEvent *event)
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

    virtual void paintEvent(QPaintEvent *event)
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

protected:
    FramelessHelper *m_pHelper;
    TitleBar *m_pTitleBar;
    QWidget *m_pMainWindow;
    QVBoxLayout *m_pMainLayout;
    QVBoxLayout *m_pFrameLessWindowLayout;
    QWidget *m_pCentralWdiget;
    bool m_redrawPixmap;          //是否需要重新创建客户区图像
    QPixmap *m_drawedPixmap;      //画好的背景图像
    QHash<QObject*, QPixmap*> m_alphaCache; //保存子控件alpha透明后的背景图
    QPixmap  m_clientPixmap;         //背景图片
    ClientDrawType m_clientDrawType; //背景图片绘制方式
    BorderImage m_borderImage;       //阴影边框
};

#endif // WIDGETSHADOW_H
