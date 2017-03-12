TARGET=virainfo

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
    virainfoplugin.h \
    virainfowidget.h \
    virastatusbar.h

SOURCES += \
    virainfoplugin.cpp \
    virainfowidget.cpp \
    virastatusbar.cpp

RESOURCES += \
    virainfopluginres.qrc

FORMS += \
    virastatusbar.ui




