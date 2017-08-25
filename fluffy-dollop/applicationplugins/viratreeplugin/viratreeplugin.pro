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
        -lregionbiz \
        -lembeddedwidgets

HEADERS += \
    viratreeplugin.h \
    viratreewidget.h \
    delegate.h \
    area_treewidget_items.h \
    addbaseareaform.h

SOURCES += \
    viratreeplugin.cpp \
    viratreewidget.cpp \
    delegate.cpp \
    area_treewidget_items.cpp \
    addbaseareaform.cpp

RESOURCES += \
    viratreepluginres.qrc

FORMS += \
    addbaseareaform.ui




