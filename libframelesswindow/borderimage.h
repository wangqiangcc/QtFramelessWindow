#ifndef BORDERIMAGE_H
#define BORDERIMAGE_H

#include <QPixmap>
#include <QMargins>

class BorderImage
{
public:
    const QMargins& margin() const { return m_margin; }
    const QMargins& border() const { return m_border; }
    const QPixmap&  pixmap() const { return m_pixmap; }
    const QString&  pixmap_url() const { return m_pixmapUrl; }

public:
    void setPixmap(const QString& url);
    void setPixmap(const QPixmap& pixmap);

    //order: left top right bottom
    void setBorder(const QString& border);
    void setBorder(const QMargins& border);
    void setBorder(int left, int top, int right, int bottom);

    void setMargin(const QString& border);
    void setMargin(const QMargins& border);
    void setMargin(int left, int top, int right, int bottom);

    void load(const QString& pixmap_url, const QString& border, const QString& margin);

private:
    QMargins m_margin;
    QMargins m_border;
    QPixmap  m_pixmap;
    QString  m_pixmapUrl;
};

#endif // BORDERIMAGE_H
