TARGET=floorgraphmakerplugin

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
    floorgraphmakerplugin.h \
    instrumentalform.h \
    floorgraphmakerstate.h \
    floorgraphelement.h \
    elementpropertyform.h

SOURCES += \
    floorgraphmakerplugin.cpp \
    instrumentalform.cpp \
    floorgraphmakerstate.cpp \
    floorgraphelement.cpp \
    elementpropertyform.cpp

RESOURCES += \
    res.qrc

FORMS += \
    instrumentalform.ui





