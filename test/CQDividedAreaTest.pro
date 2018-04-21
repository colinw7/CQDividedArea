TEMPLATE = app

TARGET = CQDividedAreaTest

DEPENDPATH += .

INCLUDEPATH += ../include .

QMAKE_CXXFLAGS += -std=c++14

CONFIG += debug

MOC_DIR = .moc

QT += widgets

# Input
SOURCES += \
CQDividedAreaTest.cpp \

HEADERS += \
CQDividedAreaTest.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

unix:LIBS += \
-L$$LIB_DIR \
-L../../CQUtil/lib \
-lCQDividedArea \
-lCQUtil \
