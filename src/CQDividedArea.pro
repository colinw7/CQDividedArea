TEMPLATE = lib

TARGET = CQDividedArea

DEPENDPATH += .

QT += widgets

CONFIG += staticlib

# Input
HEADERS += \
../include/CQDividedArea.h \

SOURCES += \
CQDividedArea.cpp \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
. \
../include \
../../CQUtil/include \
