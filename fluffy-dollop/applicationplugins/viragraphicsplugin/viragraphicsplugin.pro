TARGET=viragraphics

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
#        -lpoppler-qt5 \

HEADERS += \
    viragraphicsplugin.h \
    scenevirawidget.h \
    virapanel.h \
    viraeditorform.h \
    virapageslistwidget.h \
    viraeditorview.h \
    types.h \
    xmlreader.h \
    roomgraphicstem.h \
    workstate.h \
    areagraphicsitem.h \
    locationitem.h \
    markgraphicsitem.h

SOURCES += \
    viragraphicsplugin.cpp \
    scenevirawidget.cpp \
    virapanel.cpp \
    viraeditorform.cpp \
    virapageslistwidget.cpp \
    viraeditorview.cpp \
    xmlreader.cpp \
    roomgraphicstem.cpp \
    workstate.cpp \
    areagraphicsitem.cpp \
    locationitem.cpp \
    markgraphicsitem.cpp

RESOURCES += \
    viragraphicspluginres.qrc

FORMS += \
    viraeditorform.ui


