
#include "statebutton.h"
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QStyle>

StateButton::StateButton(QWidget *parent)
	:QPushButton(parent)
    ,m_status(NORMAL)
    ,m_mousePressed(false)
    ,m_pixmapType(NONE)
{
    setStyleSheet("QPushButton{background:transparent;border:none;}");
}

StateButton::~StateButton()
{
}

void StateButton::loadPixmap(const QString& pic_name, int state_count)
{
    m_pixmapType = FOREGROUND;
    m_pixmap.load(pic_name);
    m_stateCount = state_count;
	m_width       = m_pixmap.width()/state_count;
	m_height      = m_pixmap.height();
	setFixedSize(m_width, m_height);
}

void StateButton::setPixmap( const QPixmap& pixmap, int state_count )
{
    m_pixmapType = FOREGROUND;
    m_pixmap      = pixmap;
    m_stateCount = state_count;
    m_width       = m_pixmap.width()/state_count;
    m_height      = m_pixmap.height();
    setFixedSize(m_width, m_height);
}

void StateButton::loadBackground( const QString& pic_name, int state_count/*=4*/ )
{
    loadPixmap(pic_name, state_count);
    m_pixmapType = BACKGROUND;
}

void StateButton::setBackground( const QPixmap& pixmap, int state_count/*=4*/ )
{
    setPixmap(pixmap, state_count);
    m_pixmapType = BACKGROUND;
}

void StateButton::enterEvent(QEvent *e)
{
    m_status = HOVER;
    update();
}

void StateButton::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_mousePressed = true;
		m_status = PRESSED;
		update();
	}

    QPushButton::mousePressEvent(event);
}

void StateButton::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_mousePressed)
	{
        m_mousePressed = false;
        
        if (this->rect().contains(event->pos()))
        {
            //if (isCheckable())
            //{
            //    setChecked(!isChecked());
            //}
            //emit clicked();

            m_status = HOVER;
        }else{
            m_status = NORMAL;
        }

        update();
	}

    QPushButton::mouseReleaseEvent(event);
}

void StateButton::leaveEvent(QEvent *e)
{
	m_status = NORMAL;
	update();
    QPushButton::leaveEvent(e);
}

void StateButton::paintEvent(QPaintEvent *e)
{
    if (m_pixmapType == BACKGROUND) {
        paint_pixmap();
    }

   // QPushButton::paintEvent(e);
    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    if (option.state & QStyle::State_HasFocus) {
        option.state ^= QStyle::State_HasFocus;    //去除焦点框
        option.state |= QStyle::State_MouseOver;
    }
    p.drawControl(QStyle::CE_PushButton, option);

    if (m_pixmapType == FOREGROUND) {
        paint_pixmap();
    }
}

void StateButton::paint_pixmap()
{
    QPainter painter(this);

    //根据状态显示图片
    ButtonStatus status = m_status;
    if (!isEnabled()) {
        status = DISABLED;
    }
    else if (isChecked()) {
        status = CHECKED;
        //没有checked图片，用hover代替
        if (status >= m_stateCount) {
            status = HOVER;
        }
    }

    if (status >= m_stateCount) {
        status = NORMAL;
    }

    painter.drawPixmap(rect(), m_pixmap.copy(m_width * status, 0, m_width, m_height));
}
//////////////////////////////////////////////////////////////////////////


IconTextButton::IconTextButton( QWidget *parent )
    :QPushButton(parent)
{
}

IconTextButton::~IconTextButton()
{
}

void IconTextButton::paintEvent( QPaintEvent *e )
{
    QStyleOptionButton opt;
    initStyleOption(&opt);
    //opt.text = "";
    //opt.icon = QIcon();
    //opt.state &= ~QStyle::State_HasFocus;   //去除焦点框

    QStylePainter p(this);
    //绘制QSS描述的如border-image等
    p.drawControl(QStyle::CE_PushButtonBevel, opt);

    QRect r = rect(); //style()->subElementRect ( QStyle::SE_PushButtonContents, &opt, this);

    int v_space = 2;
    int x = r.left() + (r.width() - opt.iconSize.width()) / 2;
    int h = opt.iconSize.height() + v_space + opt.fontMetrics.height();
    int y = r.top()  + (r.height() - h) / 2;

    //y -= 2;

    p.drawPixmap(x, y, opt.icon.pixmap(opt.iconSize));

    y += opt.iconSize.height() + v_space;
    y += opt.fontMetrics.ascent();  //加这个值的原因：The y-position is used as the baseline of the font.
    x = r.left() + (r.width() - opt.fontMetrics.width(opt.text)) / 2;
    p.drawText(x, y, opt.text);
}

TextButton::TextButton(QWidget *parent)
    :QPushButton(parent)
{
    this->setFlat( true );
    this->setStyleSheet("QPushButton{background: transparent;}" );
}

TextButton::~TextButton()
{

}
