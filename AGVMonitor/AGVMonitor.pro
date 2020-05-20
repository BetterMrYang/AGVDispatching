#-------------------------------------------------
#
# Project created by QtCreator 2020-01-08T17:24:46
#
#-------------------------------------------------

QT       += core gui xml sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AGVMonitor
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
        agvmonitor.cpp \
    mygraphicsview.cpp \
    mydatabase.cpp \
    myqsqltablemodel.cpp \
    myserialport.cpp \
    dialog_viewstatus.cpp \
    thread_dealserialport.cpp \
    dialog_agvmanagement.cpp \
    Traffic/mydijkstra.cpp

HEADERS += \
        agvmonitor.h \
    mygraphicsview.h \
    mydatabase.h \
    myqsqltablemodel.h \
    myserialport.h \
    dialog_viewstatus.h \
    thread_dealserialport.h \
    dialog_agvmanagement.h \
    include.h \
    Traffic/mydijkstra.h

FORMS += \
        agvmonitor.ui \
    myserialport.ui \
    dialog_viewstatus.ui \
    dialog_agvmanagement.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
