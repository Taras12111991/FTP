#-------------------------------------------------
#
# Project created by QtCreator 2017-10-23T21:28:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FTP
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    log.cpp \
    ftpcmd.cpp \
    common.cpp \
    uev.cpp \
    ftpserver.cpp \
    tftpcmd.cpp \
    io.cpp \
    signal.cpp \
    cron.cpp \
    timer.cpp \
    dir.cpp \
    fparseln.cpp \
    strlcpy.cpp \
    strlcat.cpp \
    pidfile.cpp \
    uftpd.cpp

HEADERS  += mainwindow.h \
    uftpd.h \
    uev.h \
    private.h \
    queue.h \
    lite.h \
    strlite.h \
    strdupa.h \
    strndupa.h \
    strnlen.h \
    editor.h \
    gul.h \
    config.h \
    buffer_gap.h \
    ftpserver.h

FORMS    += mainwindow.ui
