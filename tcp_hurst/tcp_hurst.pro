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

INCLUDEPATH +=C:/Qt/Qt5.3.1/Tools/QtCreator/lib/fftw\
              C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/include\
              C:/Qt/Qt5.3.1/Tools/QtCreator/lib/gsl-2.2.1-shared/include


LIBS +=  C:/Qt/Qt5.3.1/Tools/QtCreator/lib/fftw/libfftw3-3.a\
         C:/Qt/Qt5.3.1/Tools/QtCreator/lib/fftw/libfftw3f-3.a\
         C:/Qt/Qt5.3.1/Tools/QtCreator/lib/fftw/libfftw3l-3.a \
         C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-wnd.a\
         C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-wnd.dll.a\
         C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-qt5.a\
         C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-qt.a\
         C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-qt.dll.a\
         C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-qt5.dll.a\
         C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-wx.a\
         C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-fltk.a\
        C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-glut.a\
        C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-wx.dll.a\
        C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-fltk.dll.a\
        C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl-glut.dll.a\
        C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl.a\
        C:/Qt/Qt5.3.1/Tools/QtCreator/lib/mathgl-2.4.2-mingw.win64/lib/libmgl.dll.a\
        #C:/Qt/Qt5.3.1/Tools/QtCreator/lib/gsl-2.2.1-shared/lib/libgsl.la\
        C:/Qt/Qt5.3.1/Tools/QtCreator/lib/gsl-2.2.1-shared/lib/libgsl.dll.a\
        #C:/Qt/Qt5.3.1/Tools/QtCreator/lib/gsl-2.2.1-shared/lib/libgslcblas.la\
        C:/Qt/Qt5.3.1/Tools/QtCreator/lib/gsl-2.2.1-shared/lib/libgslcblas.dll.a

# LIBS += -LC:/gsl-1.11/bin -llibgsl-0 -llibgslcblas-0
# LIBS += -L/usr/local/lib example.o -lgsl -lgslcblas -lm


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


