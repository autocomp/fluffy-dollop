TARGET=pixelvisplugin

include(../typeplugin.pri)
ROOT_DIR=../../

include($${ROOT_DIR}pri/plugins.pri)

QT += network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L$${ROOT_DIR}bin/lib \
        -lctrcore \
        -lctrvisual \
        -lctrrasterrender \

SOURCES += \
    *.cpp \

HEADERS += \
    *.h \

DISTFILES += pixelvisualizer.json

