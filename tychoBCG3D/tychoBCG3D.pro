#-------------------------------------------------
#
# Project created by QtCreator 2013-02-09T13:26:27
#
#-------------------------------------------------

QT       += core gui opengl xml

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tychoBCG3D
TEMPLATE = app

RC_FILE = myapp.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    help.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    help.h \
    about.h

FORMS    += mainwindow.ui \
    help.ui \
    about.ui

#INCLUDEPATH += /home/kapf/tychoCTG/tychoCTG/QGLViewer
LIBS += -lQGLViewer -lGLU
