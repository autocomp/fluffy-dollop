TARGET=virainfo

include(../typeplugin.pri)
ROOT_DIR=../../
include(../../pri/common.pri)

include($${ROOT_DIR}pri/plugins.pri)

QT += core gui xml widgets sql

QMAKE_CXXFLAGS += -std=c++11

DEFINES += EXTERNAL_USING

#LIBS += -L$${ROOT_DIR}bin/lib \
LIBS += -L$${INSTALL_PATH_LIB} \
        -lctrcore \
        -lctrvisual \
        -lctrwidgets \
        -lregionbiz \

HEADERS += \
    virainfoplugin.h \
    virainfowidget.h \
    virastatusbar.h \
    test_data_getter.h \
    markform.h

SOURCES += \
    virainfoplugin.cpp \
    virainfowidget.cpp \
    virastatusbar.cpp \
    test_data_getter.cpp \
    markform.cpp

RESOURCES += \
    virainfopluginres.qrc

FORMS += \
    virastatusbar.ui \
    virainfowidget.ui \
    markform.ui




