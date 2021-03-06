#-------------------------------------------------
#
# Project created by QtCreator 2014-11-22T16:51:35
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = didftp
TEMPLATE = app


SOURCES += main.cpp\
        login.cpp \
    mainwindow.cpp \
    upload.cpp \
    monitor.cpp \
    qftp.cpp \
    qurlinfo.cpp \
    createdir.cpp \
    subtitle.cpp

HEADERS  += login.h \
    mainwindow.h \
    upload.h \
    monitor.h \
    qftp.h \
    qurlinfo.h \
    createdir.h \
    subtitle.h

FORMS    += login.ui \
    mainwindow.ui \
    upload.ui \
    monitor.ui \
    createdir.ui \
    subtitle.ui

RESOURCES += \
    resource.qrc

TRANSLATIONS += \
    didftp_ko.ts

OTHER_FILES +=
