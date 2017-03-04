#-------------------------------------------------
#
# Project created by QtCreator 2016-02-02T16:11:08
#
#-------------------------------------------------

TARGET=ctrvisual

ROOT_DIR=../

TEMPLATE = lib

CONFIG += plugin

include($${ROOT_DIR}/pri/common.pri)

QT += core gui network opengl xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    state/*.cpp \
    scenewidget/*.cpp \
    scenecontrol/*.cpp \
    components/*.cpp \

HEADERS += \
    state/*.h \
    scenewidget/*.h \
    scenecontrol/*.h \
    components/*.h \

#TRANSLATIONS += ctrvisual.ts

FORMS += \
#    scenecontrol/providersaveprocessingdialog.ui

LIBS += -L$${ROOT_DIR}bin/lib \
        -lctrcore \
        -lctrrasterrender \
        -lGeographic \
#        -ldpf_core \
#        -ldpf_geo \
#        -ldpf_io_base \
#        -ldpf_srv_immeasure \
#        -lboost_program_options \
#        -lboost_date_time \
#        -lboost_system \
#        -lobjrepr \

include ($${ROOT_DIR}/pri/install.pri)

RESOURCES += \
    scenecontrol/scenecontrolresources.qrc \
    scenewidget/scenewidgetstyle.qrc \
    components/components.qrc \
    ctrvisual.qrc

OTHER_FILES += \
    scenewidget/blackStyle.qss

DISTFILES +=
