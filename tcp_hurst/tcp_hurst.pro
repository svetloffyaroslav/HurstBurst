#-------------------------------------------------
#
# Project created by QtCreator 2017-09-27T16:30:07
#
#-------------------------------------------------

QT       += core gui printsupport network multimedia testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HurstBurst
TEMPLATE = app
RC_FILE = icon.rc
CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    test_gui.cpp \
    setcolorandsettingsgraph.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    test_gui.h \
    setcolorandsettingsgraph.h

FORMS    += mainwindow.ui \
    setcolorandsettingsgraph.ui

RESOURCES += \
    resources.qrc


