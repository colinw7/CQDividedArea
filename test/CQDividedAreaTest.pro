TEMPLATE = app

TARGET = 

DEPENDPATH += .

#CONFIG += debug

# Input
SOURCES += \
CQDividedAreaTest.cpp \

HEADERS += \
CQDividedAreaTest.h \

DESTDIR     = .
OBJECTS_DIR = .

INCLUDEPATH += \
../include \
.

unix:LIBS += \
-L../lib \
-lCQDividedArea
