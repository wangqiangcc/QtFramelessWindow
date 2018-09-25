/**
 * 自定义无边框窗体、对话框和提示框并封装成库/测试程序
 *
 * mainwindow.cpp
 * 测试窗体。
 *
 */

#include "mainwindow.h"
#include <QPushButton>
#include <QLayout>
#include <QPainter>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : FramelessWindow(parent)
{
    setWindowTitle(tr("Custom Frameless Window"));
    setRubberBandOnMove(false);
    hideTitleBar();

    setStyleSheetFile(":/style/style_white.qss");

    QWidget *pMainWidget = new QWidget(this);

    QWidget *leftWidget = new QWidget(this);
    leftWidget->setFixedWidth(64);
    leftWidget->setStyleSheet("background-color:#03A9F4;");

    QWidget *rightWidget = new QWidget(this);
    rightWidget->setStyleSheet("background-color:#EEEEEE;");

    TitleBar *titleBar = new TitleBar(this);

    QWidget *rigthContext = new QWidget(this);
    rigthContext->setStyleSheet("background-color:#FFFFFF;");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(titleBar);
    rightLayout->addWidget(rigthContext);
    rightLayout->addStretch();


    QHBoxLayout *pLayout = new QHBoxLayout(pMainWidget);
    pLayout->addWidget(leftWidget);
    pLayout->addWidget(rightWidget);
    pLayout->setMargin(0);
    pLayout->setSpacing(0);

    setCentralWidget(pMainWidget);

}
