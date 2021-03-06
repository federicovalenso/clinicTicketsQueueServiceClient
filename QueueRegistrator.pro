#-------------------------------------------------
#
# Project created by QtCreator 2018-05-24T12:12:45
#
#-------------------------------------------------

QT       += core gui network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QueueRegistrator
TEMPLATE = app

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dlgsignin.cpp \
    appsettings.cpp \
    requestsprocessor.cpp \
    dlgsettings.cpp \
    ticketsprocessor.cpp \
    dlgchooseticket.cpp \
    models/ticketsmodel.cpp \
    states/appstate.cpp \
    states/backgroundstate.cpp \
    states/nextticketstate.cpp \
    states/closestate.cpp \
    states/returnticketstate.cpp \
    states/voiceticketstate.cpp \
    states/selectticketstate.cpp

HEADERS += \
        mainwindow.h \
    dlgsignin.h \
    appsettings.h \
    requestsprocessor.h \
    dlgsettings.h \
    ticketsprocessor.h \
    ticket.h \
    dlgchooseticket.h \
    models/ticketsmodel.h \
    states/appstate.h

FORMS += \
        mainwindow.ui \
    dlgsignin.ui \
    dlgsettings.ui \
    dlgchooseticket.ui

win32:RC_ICONS += icon.ico

RESOURCES += \
    res.qrc
