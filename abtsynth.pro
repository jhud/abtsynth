#-------------------------------------------------
#
# Project created by QtCreator 2012-04-03T11:06:46
#
#-------------------------------------------------

QT       += core gui opengl xml widgets

TARGET = abtsynth
TEMPLATE = app

mac {
    LIBS += -L/opt/X11/lib/
}

LIBS += -lGLU

INCLUDEPATH += src

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/glviewwidget.cpp \
    src/bone.cpp \
    src/skeleton.cpp \
    src/capsule.cpp \
    src/maths.cpp \
    src/ribbon.cpp \
    src/branch.cpp \
    src/branchfollowingribbon.cpp \
    src/debugshapes.cpp

HEADERS  += src/mainwindow.h \
    src/glviewwidget.h \
    src/bone.h \
    src/skeleton.h \
    src/capsule.h \
    src/common.h \
    src/maths.h \
    src/ribbon.h \
    src/branch.h \
    src/branchfollowingribbon.h \
    src/debugshapes.h

FORMS    += \
    src/mainwindow.ui

OTHER_FILES += \
    data/guy.xml \
    data/guyandgirl.xml \
    data/bird.xml \
    data/triangle.xml \
    data/sphere.xml \
    data/capsule.xml \
    data/lozenge.xml
