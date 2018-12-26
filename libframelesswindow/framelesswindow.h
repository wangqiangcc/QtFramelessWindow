/**
 * 自定义无边框窗体、对话框和提示框并封装成库
 *
 * framelesswindow.h
 * 定义了FramelessWindow、FramlessDialog、FramelessMessageBox
 *
 */

#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include "framelesswindow_global.h"
#include "borderimage.h"
#include "widgetshadow.h"
#include <QtWidgets>
#include <QDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QEventLoop>

typedef WidgetShadow<QWidget> FramelessWindow;

/**
 * @brief The FramelessDialog class
 *  无边框自定义对话框
 */
class FRAMELESSWINDOWSHARED_EXPORT FramelessDialog : public WidgetShadow<QDialog>
{
    Q_OBJECT

public:
    FramelessDialog(QWidget *parent = nullptr);

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

    enum IconType {
        MSG_NOICON = 0,  // 无图标
        MSG_INFORMATION, // 提示信息
        MSG_WARNNING,    // 提示警告
        MSG_QUESTION,    // 提示询问
        MSG_ERROR,       // 提示错误
        MSG_SUCCESS,     // 提示成功
    };

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
     * @note 设置提示信息
     * @param text
     */
    void setText(const QString &text);
    /**
     * @brief setIcon
     * @note 设置窗体图标
     * @param icon
     */
    void setIcon(const QString &icon);

    /**
     * @brief hideInfoIcon
     * @note 隐藏icon
     */
    void hideInfoIcon();

    /**
     * @brief addWidget
     *  添加控件-替换提示信息所在的QLabel
     * @param pWidget
     */
    void addWidget(QWidget *pWidget);

    static QMessageBox::StandardButton showMessageBox(QWidget *parent, const QString &title,
                                                      const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                      IconType messageType = MSG_NOICON,
                                                      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

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
