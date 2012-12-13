#-------------------------------------------------
#
# Project created by QtCreator 2012-11-23T09:42:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DSLRVideoLab
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dslrlabview.cpp \
    ffsequence.cpp \
    textpill.cpp

HEADERS  += mainwindow.h \
    dslrlabview.h \
    ffsequence.h \
    textpill.h

FORMS    += mainwindow.ui

unix|win32: LIBS += -lavformat
