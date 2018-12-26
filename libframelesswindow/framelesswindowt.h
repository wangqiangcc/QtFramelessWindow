/**
 * 自定义无边框窗体、对话框和提示框并封装成库
 *
 * framelesswindowt.h
 * 定义了FramelessWindow、FramlessDialog、FramelessMessageBox
 *
 */

#ifndef FRAMELESSWINDOWT_H
#define FRAMELESSWINDOWT_H

#include "borderimage.h"
#include <QWidget>
#include <QDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QEventLoop>

class FramelessHelper;
class TitleBar;

/**
 * @brief The FramelessWindow class
 *  无边框自定义窗体
 */

class FramelessWindow : public QWidget
{
    Q_OBJECT

public:
    FramelessWindow(QWidget *parent = nullptr);
    ~FramelessWindow();

    enum ClientDrawType {
        kTopLeftToBottomRight = 1,  //左下到右下
        kTopRightToBottomLeft       //右上到左下
    };

    /**
     * @brief setStyleSheetFile
     * @note 设置QSS样式文件
     * @param file
     */
    void setStyleSheetFile(const QString &file);

    /**
     * @brief setTitleHeight
     * @note 设置标题栏高度，如果设置了窗口可移动，拖动标题栏可以移动窗体
     * @param h
     * @note 标题栏的高度,默认是25
     */
    void setTitleHeight(int h = 25);

    /**
     * @brief setWidgetMovalbe
     * @note 设置窗口是否可移动，默认可移动
     * @param movable
     */
    void setWidgetMovalbe(bool movable = true);

    /**
     * @brief setWidgetResizable
     * @note 设置窗口是否可缩放，默认是可以进行缩放
     * @param resizable
     */
    void setWidgetResizable(bool resizable = true);

    /**
     * @brief setMinimumVisible
     * @note 设置窗口标题栏最小化按钮是否可见
     * @param vislble
     */
    void setMinimumVisible(bool vislble = true);

    /**
     * @brief setMaximumVisible
     * @note 设置窗口标题栏最大化或还原按钮是否可见
     * @param visible
     */
    void setMaximumVisible(bool visible = true);

    /**
     * @brief setRubberBandOnMove
     * @note 设置窗口缩放时橡皮筋是否可移动，默认是可移动
     * @param rubber
     */
    void setRubberBandOnMove(bool move = true);

    /**
     * @brief setRubberBandOnResize
     * @note 设置窗口缩放时橡皮筋是否可缩放，默认可缩放
     * @param resize
     */
    void setRubberBandOnResize(bool resize = true);

    /**
     * @brief setCentralWidget
     * @note 设置中心界面
     * @param w
     *  QWidget *
     */
    void setCentralWidget(QWidget *w);

    /**
     * @brief setBorderImage
     * @param image
     * @note 设置边框图片或阴影
     */
    void setBorderImage(const QString &image, const QMargins& border = {8,8,8,8}, const QMargins& margin = {6,6,6,6});

    /**
     * @brief setClientImage
     * @param file
     * @note 设置窗口背景片
     */
    void setClientImage(const QString &image);

    /**
     * @brief setClientColor
     * @param color
     * @note 设置背景颜色
     */
    void setClientColor(const QColor &color);

    /**
     * @brief clientDrawType
     * @note 客户区背景绘制方式
     * @return
     */
    inline ClientDrawType clientDrawType() const { return m_clientDrawType; }
    void setClientDrawType(ClientDrawType type);

    /**
     * @brief hideTitleBar
     * @note 不显示标题栏
     */
    void hideTitleBar();

    /**
     * @brief 除去边框后的客户区rect
     * @return
     */
    QRect clientRect() const;

    /**
     * @brief 画背景图, 左上角画原始图，右下角拉伸
     * @param painter
     * @param rect
     * @param img
     */
    void drawTopLeft(QPainter *painter, const QRect& rect, const QPixmap& pixmap);

    /**
     * @brief 画背景图, 右上角画原始图，左下角拉伸
     * @param painter
     * @param rect
     * @param img
     */
    void drawTopRight(QPainter *painter, const QRect& rect, const QPixmap& pixmap);

    /**
     * @brief 事件过滤，处理paintEvent, 控制子窗口对背景图的alpha透明
     * @param object
     * @param event
     * @return
     */
    bool eventFilter(QObject *object, QEvent *event);

protected:
    //virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
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

/**
 * @brief The FramelessDialog class
 *  无边框自定义对话框
 */
class FRAMELESSWINDOWSHARED_EXPORT FramelessDialog : public QDialog
{
    Q_OBJECT

public:
    FramelessDialog(QWidget *parent = nullptr);

    enum ClientDrawType {
        kTopLeftToBottomRight = 1,  //左下到右下
        kTopRightToBottomLeft       //右上到左下
    };

    /**
     * @brief setStyleSheetFile
     * @note 设置QSS样式文件
     * @param file
     */
    void setStyleSheetFile(const QString &file);

    /**
     * @brief setTitleHeight
     * @note 设置标题栏高度，如果设置了窗口可移动，拖动标题栏可以移动窗体
     * @param h
     *  标题栏的高度,默认是25
     */
    void setTitleHeight(int h = 25);

    /**
     * @brief setWidgetMovalbe
     * @note 设置窗口是否可移动，默认可移动
     * @param movable
     */
    void setWidgetMovalbe(bool movable = true);

    /**
     * @brief setWidgetResizable
     * @note 设置窗口是否可缩放，默认是可以进行缩放
     * @param resizable
     */
    void setWidgetResizable(bool resizable = true);

    /**
     * @brief setMinimumVisible
     * @note 设置窗口标题栏最小化按钮是否可见
     * @param vislble
     */
    void setMinimumVisible(bool vislble = true);

    /**
     * @brief setMaximumVisible
     * @note 设置窗口标题栏最大化或还原按钮是否可见
     * @param visible
     */
    void setMaximumVisible(bool visible = true);

    /**
     * @brief setRubberBandOnMove
     * @note 设置窗口缩放时橡皮筋是否可移动，默认是可移动
     * @param rubber
     */
    void setRubberBandOnMove(bool move = true);

    /**
     * @brief setRubberBandOnResize
     * @note 设置窗口缩放时橡皮筋是否可缩放，默认可缩放
     * @param resize
     */
    void setRubberBandOnResize(bool resize = true);

    /**
     * @brief setCentralWidget
     * @note 设置 dialog 中心控件
     * @param w
     *  QWidget *
     */
    void setCentralWidget(QWidget *w);

    /**
     * @brief setBorderImage
     * @param image
     * @note 设置边框图片或阴影
     */
    void setBorderImage(const QString &image, const QMargins& border = {8,8,8,8}, const QMargins& margin = {6,6,6,6});

    /**
     * @brief setClientImage
     * @param file
     * @note 设置窗口背景片
     */
    void setClientImage(const QString &image);

    /**
     * @brief setClientColor
     * @param color
     * @note 设置背景颜色
     */
    void setClientColor(const QColor &color);

    /**
     * @brief clientDrawType
     * @note 客户区背景绘制方式
     * @return
     */
    inline ClientDrawType clientDrawType() const { return m_clientDrawType; }
    void setClientDrawType(ClientDrawType type);

    /**
     * @brief 除去边框后的客户区rect
     * @return
     */
    QRect clientRect() const;

    /**
     * @brief 画背景图, 左上角画原始图，右下角拉伸
     * @param painter
     * @param rect
     * @param img
     */
    void drawTopLeft(QPainter *painter, const QRect& rect, const QPixmap& pixmap);

    /**
     * @brief 画背景图, 右上角画原始图，左下角拉伸
     * @param painter
     * @param rect
     * @param img
     */
    void drawTopRight(QPainter *painter, const QRect& rect, const QPixmap& pixmap);

    /**
     * @brief 事件过滤，处理paintEvent, 控制子窗口对背景图的alpha透明
     * @param object
     * @param event
     * @return
     */
    bool eventFilter(QObject *object, QEvent *event);

protected:
    //virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

protected:
    QVBoxLayout *m_pFrameLessWindowLayout;

private:
    FramelessHelper *m_pHelper;
    TitleBar *m_pTitleBar;
    QWidget *m_pCentralWidget;
    bool m_redrawPixmap;          //是否需要重新创建客户区图像
    QPixmap *m_drawedPixmap;      //画好的背景图像
    QHash<QObject*, QPixmap*> m_alphaCache; //保存子控件alpha透明后的背景图
    QPixmap  m_clientPixmap;         //背景图片
    ClientDrawType m_clientDrawType; //背景图片绘制方式
    BorderImage m_borderImage;       //阴影边框
};


class QLabel;
class QGridLayout;
class QDialogButtonBox;
class QHBoxLayout;
class QAbstractButton;
/**
 * @brief The FramelessMessageBox class
 *  无边框自定义提示框
 */
class FRAMELESSWINDOWSHARED_EXPORT FramelessMessageBox : public FramelessDialog
{
    Q_OBJECT

public:
    FramelessMessageBox(QWidget *parent = nullptr, const QString &title = tr("Tip"), const QString &text = "",
               QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::Ok);

    ~FramelessMessageBox();
    QAbstractButton *clickedButton() const;
    QMessageBox::StandardButton standardButton(QAbstractButton *button) const;
    /**
     * @brief setDefaultButton
     *  设置默认按钮
     * @param button
     */
    void setDefaultButton(QPushButton *button);
    void setDefaultButton(QMessageBox::StandardButton button);
    /**
     * @brief setTitle
     *  设置窗体标题
     * @param title
     */
    void setTitle(const QString &title);
    /**
     * @brief setText
     *  设置提示信息
     * @param text
     */
    void setText(const QString &text);
    /**
     * @brief setIcon
     *  设置窗体图标
     * @param icon
     */
    void setIcon(const QString &icon);
    /**
     * @brief addWidget
     *  添加控件-替换提示信息所在的QLabel
     * @param pWidget
     */
    void addWidget(QWidget *pWidget);

    static QMessageBox::StandardButton showInformation(QWidget *parent, const QString &title,
                                                const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton showError(QWidget *parent, const QString &title,
                                                 const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                 QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton showSuccess(QWidget *parent, const QString &title,
                                                   const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                   QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton showQuestion(QWidget *parent, const QString &title,
                                             const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No,
                                             QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton showWarning(QWidget *parent, const QString &title,
                                                   const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                   QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton showCritical(QWidget *parent, const QString &title,
                                                    const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                    QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    static QMessageBox::StandardButton showCheckBoxQuestion(QWidget *parent, const QString &title,
                                                            const QString &text, QMessageBox::StandardButtons buttons,
                                                            QMessageBox::StandardButton defaultButton);


protected:
    /**
     * @brief changeEvent
     *  多语言翻译
     * @param event
     */
    void changeEvent(QEvent *event);

private slots:
    void onButtonClicked(QAbstractButton *button);

private:
    void translateUI();
    int execReturnCode(QAbstractButton *button);

private:
    QLabel *m_pIconLabel;
    QLabel *m_pLabel;
    QGridLayout *m_pGridLayout;
    QDialogButtonBox *m_pButtonBox;
    QAbstractButton *m_pClickedButton;
    QAbstractButton *m_pDefaultButton;
    QHBoxLayout *m_pLayout;
};

#endif // FRAMELESSWINDOW_H
