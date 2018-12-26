#include "borderimage.h"
#include <QTextStream>

void BorderImage::setPixmap(const QString& url)
{
    m_pixmapUrl = url;
    m_pixmap.load(url);
}

void BorderImage::setPixmap(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
}

void BorderImage::setBorder(const QString& border)
{
    QTextStream qs((QString*)&border);
    int left, top, right, bottom;
    qs >> left >> top >> right >> bottom;
    setBorder(left, top, right, bottom);
}

void BorderImage::setBorder(const QMargins& border)
{
    m_border = border;
}

void BorderImage::setBorder(int left, int top, int right, int bottom)
{
    QMargins m(left, top, right, bottom);
    m_border = m;
}

void BorderImage::setMargin(const QString& border)
{
    QTextStream qs((QString*)&border);
    int left, top, right, bottom;
    qs >> left >> top >> right >> bottom;
    setMargin(left, top, right, bottom);
}

void BorderImage::setMargin(const QMargins& border)
{
    m_margin = border;
}

void BorderImage::setMargin(int left, int top, int right, int bottom)
{
    QMargins m(left, top, right, bottom);
    m_margin = m;
}

void BorderImage::load(const QString& pixmap_url, const QString& border, const QString& margin)
{
    setPixmap(pixmap_url);
    setBorder(border);
    setMargin(margin);
}
