#-------------------------------------------------
#
# Project created by QtCreator 2012-04-03T11:06:46
#
#-------------------------------------------------

QT       += core gui opengl xml

TARGET = charabesque
TEMPLATE = app

LIBS += -lGLU

INCLUDEPATH += src

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/glviewwidget.cpp \
    src/bone.cpp \
    src/skeleton.cpp \
    src/spark.cpp \
    src/capsule.cpp \
    src/maths.cpp \
    src/ribbon.cpp

HEADERS  += src/mainwindow.h \
    src/glviewwidget.h \
    src/bone.h \
    src/skeleton.h \
    src/spark.h \
    src/capsule.h \
    src/common.h \
    src/maths.h \
    src/ribbon.h

FORMS    += \
    src/mainwindow.ui

OTHER_FILES += \
    data/guy.xml
