#-------------------------------------------------
#
# Project created by QtCreator 2016-02-02T16:11:08
#
#-------------------------------------------------

TARGET=ctrrasterrender

ROOT_DIR=../

TEMPLATE = lib
CONFIG += plugin

include($${ROOT_DIR}/pri/common.pri)

QMAKE_CXXFLAGS += -std=c++11

QT += xml network opengl

LIBS += -L$${INSTALL_PATH_LIB} \
        -lctrcore \

SOURCES += \
    $$files(tms/*.cpp) \
    $$files(image/*.cpp) \

HEADERS += \
    $$files(tms/*.h) \
    $$files(image/*.h) \

DISTFILES += \

SUBDIRS += \

include ($${ROOT_DIR}/pri/install.pri)

RESOURCES += \
#    gdal/gdalresources.qrc

