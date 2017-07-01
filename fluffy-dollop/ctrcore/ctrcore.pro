#-------------------------------------------------
#
# Project created by QtCreator 2016-02-02T16:11:08
#
#-------------------------------------------------
TARGET=ctrcore

ROOT_DIR = ../

TEMPLATE = lib

CONFIG += plugin

include($${ROOT_DIR}/pri/common.pri)

QMAKE_CXXFLAGS += -std=c++11

QT += core gui network sql xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES +=     \
    $$files(provider/*.cpp) \
    $$files(plugin/*.cpp) \
    $$files(visual/*.cpp) \
    $$files(ctrcore/*.cpp) \
    $$files(bus/*.cpp) \
    $$files(undoactions/*.cpp) \
    $$files(tempinputdata/*.cpp) \

HEADERS += \
    $$files(provider/*.h) \
    $$files(plugin/*.h) \
    $$files(visual/*.h) \
    $$files(ctrcore/*.h) \
    $$files(bus/*.h) \
    $$files(undoactions/*.h) \
    $$files(tempinputdata/*.h) \

#TRANSLATIONS += ctrcore.ts

LIBS += -L$${INSTALL_PATH_LIB} \
        -lembeddedwidgets \
#        -lGeographic \
#        -lgdal \
#        -lunilog \
#        -lobjrepr \
#        -ldpf_io_base \
#        -ldpf_core \
#        -ldpf_geo \
#        -ldpf_math \
#        -ldpf_improc \

DEFINES += MODULE_NAME=\\\"$$TARGET\\\"

include ($${ROOT_DIR}/pri/install.pri)

RESOURCES += \
    visual/visualresources.qrc \
    ctrcore.qrc

