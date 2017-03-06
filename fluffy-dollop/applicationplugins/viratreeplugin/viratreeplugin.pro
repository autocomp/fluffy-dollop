TARGET=viratree

include(../typeplugin.pri)
ROOT_DIR=../../

include($${ROOT_DIR}pri/plugins.pri)

QT += core gui xml widgets sql

QMAKE_CXXFLAGS += -std=c++11

DEFINES += EXTERNAL_USING

#LIBS += -L$${ROOT_DIR}bin/lib \
LIBS += -L$${INSTALL_PATH_LIB} \
        -lctrcore \
        -lctrvisual \
        -lctrwidgets \
        -lgdal \
        -lregionbiz \

HEADERS += \
    viratreeplugin.h \
    viratreewidget.h

SOURCES += \
    viratreeplugin.cpp \
    viratreewidget.cpp

RESOURCES += \
    viratreepluginres.qrc




