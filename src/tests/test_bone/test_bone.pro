#-------------------------------------------------
#
# Project created by QtCreator 2012-04-04T20:27:28
#
#-------------------------------------------------

QT       += testlib gui

TARGET = tst_test_bone
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_test_bone.cpp \
    ../../bone.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../bone.h
