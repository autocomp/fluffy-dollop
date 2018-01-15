ROOT_DIR=../
include( ../pri/common.pri )

## qmake project for panini ##
TEMPLATE = lib
VERSION = 0.72.0
TARGET = panini
CONFIG += debug_and_release plugin
QT = gui core opengl
DESTDIR = $$INSTALL_PATH_LIB

LIBS += -lz

# We want Qt5 now
lessThan(QT_MAJOR_VERSION, 5): error("requires Qt 5")

## Platform specific build settings.... ##

win32 {
# this is just for zlib, change if necessary...
    INCLUDEPATH += c:/MinGW/GnuWin32/include
# this sets the program icon the Windows way...
    RC_FILE = ui/paniniWin.rc
# add GLU
    LIBS += -lGLU
}

## Version Number ##
DEFINES += VERSION=\\\"$$VERSION\\\"

HEADERS += \
    panocylinder.h \
    panosphere.h \
    panosurface.h \
    pvQtPic.h \
    pvQt_QTVR.h \
    pvQtView.h

SOURCES += \
    panocylinder.cpp \
    panosphere.cpp \
    panosurface.cpp \
    pictureTypes.cpp \
    pvQtPic.cpp \
    pvQt_QTVR.cpp \
    pvQtView.cpp
