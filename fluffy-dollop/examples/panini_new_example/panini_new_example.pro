ROOT_DIR=../../
include( ../pri/common.pri )

## qmake project for panini ##
TEMPLATE = app
VERSION = 0.72.0
TARGET = panini
CONFIG += debug_and_release
QT = gui core opengl
LIBS += -lz -lpanini

# We want Qt5 now
lessThan(QT_MAJOR_VERSION, 5): error("requires Qt 5")

## Platform specific build settings.... ##

## TODO: add option to make Mac OS unversal binaries;
##       add scripts to build distribution packages

win32 {
# this is just for zlib, change if necessary...
    INCLUDEPATH += c:/MinGW/GnuWin32/include
# this sets the program icon the Windows way...
    RC_FILE = ui/paniniWin.rc
# add GLU
    LIBS += -lGLU
}

## Source Files ##
SOURCES = main.cpp \
    mainwindow.cpp

## Version Number ##
DEFINES += VERSION=\\\"$$VERSION\\\"

FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h
