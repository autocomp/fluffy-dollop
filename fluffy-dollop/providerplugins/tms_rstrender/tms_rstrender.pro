TARGET=tms_ioplugin

include(../typeplugin.pri)
ROOT_DIR=../../

include($${ROOT_DIR}pri/plugins.pri)

SOURCES += \
    *.cpp

HEADERS += \
    *.h
