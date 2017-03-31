TARGET=tms_ioplugin

include(../typeplugin.pri)
ROOT_DIR=../../

include($${ROOT_DIR}pri/plugins.pri)

LIBS += -L$${INSTALL_PATH_LIB} \
        -lctrcore \
        -lctrrasterrender \

SOURCES += \
    *.cpp

HEADERS += \
    *.h
