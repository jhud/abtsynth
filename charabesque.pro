#-------------------------------------------------
#
# Project created by QtCreator 2012-04-03T11:06:46
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = charabesque
TEMPLATE = app

LIBS += -lGLU

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/glviewwidget.cpp

HEADERS  += src/mainwindow.h \
    src/glviewwidget.h

FORMS    += \
    src/mainwindow.ui
