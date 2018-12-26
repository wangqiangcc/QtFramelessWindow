
#ifndef _STATE_BUTTON_H_
#define _STATE_BUTTON_H_

#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>

//a pixmap have 5 picture which state is normal\hover\pressed\disabled\checked
//this button does not draw focus rect
class StateButton : public QPushButton
{
    Q_OBJECT
public:
    explicit StateButton(QWidget *parent = 0);
    ~StateButton();

public:
    void loadPixmap(const QString& pic_name, int state_count=4);
    void setPixmap(const QPixmap& pixmap, int state_count=4);

    void loadBackground(const QString& pic_name, int state_count=4);
    void setBackground(const QPixmap& pixmap, int state_count=4);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

private:
    void paint_pixmap();

private:
    //枚举按钮的几种状态
    enum ButtonStatus {NORMAL, HOVER, PRESSED, DISABLED, CHECKED};
    //pximap_位图类型, 仅能选择其一. 如果两者都需要，请选择BACKGROUND + QPushButton::setIcon
    enum PixmapType   {NONE, FOREGROUND, BACKGROUND};

    QPixmap         m_pixmap;        //图片
    PixmapType      m_pixmapType;
    int             m_stateCount;    //图片有几种状态(几张子图)
    ButtonStatus    m_status;        //当前状态
    int             m_width;         //按钮宽度
    int             m_height;        //按钮高度
    bool            m_mousePressed;  //鼠标左键是否按下
};

//按钮的图标在上面，文字在下面
class IconTextButton: public QPushButton
{
public:
    IconTextButton(QWidget *parent);
    ~IconTextButton();

protected:
    void paintEvent(QPaintEvent *e);
};

class TextButton: public QPushButton
{
    Q_OBJECT
public:
    TextButton(QWidget *parent = 0);
    ~TextButton();
};

#endif //
