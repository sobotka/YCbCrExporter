QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ycbcrexporter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ffsequence.cpp \
    textpill.cpp \
    qbasegraphicsview.cpp \
    qgraphicstrimslider.cpp \
    ycbcrlabview.cpp

HEADERS  += mainwindow.h \
    ffsequence.h \
    textpill.h \
    qbasegraphicsview.h \
    qgraphicstrimslider.h \
    ycbcrlabview.h

unix|win32: LIBS += -lavformat

unix:!symbian|win32: LIBS += -lOpenImageIO

RESOURCES +=

OTHER_FILES += \
    TODO.txt \
    LICENSE.txt \
    CMakeLists.txt \
    README.mediawiki
