TEMPLATE = app

TARGET = libTest

#DEFINES += HAVE_WINDOW_AERO

include(../libFramelessWindow/libFramelessWindow.pri)

DESTDIR = $$PROJECT_BINDIR

unix:QMAKE_RPATHDIR += $$PROJECT_LIBDIR

QT += widgets

SOURCES += \
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    mainwindow.h \
    stylesheethelper.h \

RESOURCES += \
    style.qrc

