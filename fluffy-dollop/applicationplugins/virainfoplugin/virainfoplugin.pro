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
        -lembeddedwidgets

HEADERS += \
    virainfoplugin.h \
    virainfowidget.h \
    virastatusbar.h \
    markform.h \
    data_getter.h \
    infoform.h \
    pixmaplistwidget.h \
    slide_window.h \
    filter_form.h \
    imageviewer.h \
    photo360form.h

SOURCES += \
    virainfoplugin.cpp \
    virainfowidget.cpp \
    virastatusbar.cpp \
    markform.cpp \
    data_getter.cpp \
    infoform.cpp \
    pixmaplistwidget.cpp \
    slide_window.cpp \
    filter_form.cpp \
    imageviewer.cpp \
    photo360form.cpp

RESOURCES += \
    virainfopluginres.qrc

FORMS += \
    virastatusbar.ui \
    virainfowidget.ui \
    markform.ui \
    infoform.ui \
    filter_form.ui \
    photo360form.ui




