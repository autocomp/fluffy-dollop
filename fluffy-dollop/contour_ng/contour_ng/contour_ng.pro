#-------------------------------------------------
#
# Project created by QtCreator 2016-06-15T16:18:54
#
#-------------------------------------------------
TARGET=contour_ng

ROOT_DIR = ../../

TEMPLATE = app

include($${ROOT_DIR}/pri/common.pri)

QT += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += network opengl xml


QMAKE_RPATHDIR += $${INSTALL_PATH_LIB}

LIBS += -L$${INSTALL_PATH_LIB} \
        -lctrcore \
        -lctrrasterrender \
        -lctrvisual \
        -lctrwidgets \
        -lembeddedwidgets \
        -lGeographic \
#        -lctrutils \
#        -lctrobjectwidgets \
#        -ldpf_io_base \
#        -ldpf_core \
#        -ldpf_geo \
#        -ldpf_math \
#        -ldpf_improc \
#        -ldpf_srv_sfm \
#        -ldpf_sfm \
#        -lsiftgpu \
#        -lobjrepr \
#        -lunilog \


QMAKE_RPATHDIR += $${INSTALL_PATH_LIB}

SOURCES += main.cpp\
    appcomposer.cpp

HEADERS  += \
    appcomposer.h

#TRANSLATIONS += contour_ng.ts

FORMS    +=

QMAKE_CXXFLAGS += -std=c++11

RESOURCES += \
    contour_ng.qrc

DISTFILES += \
    contour_ng.ts
