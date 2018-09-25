INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += FRAMELESSWINDOW_LIBRARY

HEADERS += \
    $$PWD/cursorposcalculator.h \
    $$PWD/framelesshelper.h \
    $$PWD/framelesshelperprivate.h \
    $$PWD/framelesswindow.h \
    $$PWD/framelesswindow_global.h \
    $$PWD/widgetdata.h \
    $$PWD/titlebar.h \
    $$PWD/borderimage.h

SOURCES += \
    $$PWD/cursorposcalculator.cpp \
    $$PWD/framelesshelper.cpp \
    $$PWD/framelesswindow.cpp \
    $$PWD/widgetdata.cpp \
    $$PWD/titlebar.cpp \
    $$PWD/borderimage.cpp

RESOURCES += \
    $$PWD/images.qrc \
    $$PWD/style.qrc
