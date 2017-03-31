TARGET=image_ioplugin

include(../typeplugin.pri)
ROOT_DIR=../../

include($${ROOT_DIR}pri/plugins.pri)

LIBS += -L$${ROOT_DIR}bin/lib \
        -lctrcore \
        -lctrrasterrender \

SOURCES += \
    $$files(*.cpp)

HEADERS += \
    $$files(*.h)
