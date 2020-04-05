#-------------------------------------------------
#
# Project created by QtCreator 2019-08-20T09:18:59
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GraphicsPractise
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dialog_input.cpp \
    dialog_attribute.cpp \
    mygraphicsview.cpp \
    dialog_search.cpp \
    dialog_firstpos.cpp \
    dialog_loop.cpp \
    floyd.cpp

HEADERS += \
        mainwindow.h \
    dialog_input.h \
    dialog_attribute.h \
    mygraphicsview.h \
    dialog_search.h \
    dialog_firstpos.h \
    dialog_loop.h \
    floyd.h

FORMS += \
        mainwindow.ui \
    dialog_input.ui \
    dialog_attribute.ui \
    dialog_search.ui \
    dialog_firstpos.ui \
    dialog_loop.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resuorce.qrc


DISTFILES +=
