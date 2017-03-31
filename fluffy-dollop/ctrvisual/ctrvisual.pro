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
    $$files(state/*.cpp) \
    $$files(scenewidget/*.cpp) \
    $$files(scenecontrol/*.cpp) \
    $$files(components/*.cpp) \

HEADERS += \
    $$files(state/*.h) \
    $$files(scenewidget/*.h) \
    $$files(scenecontrol/*.h) \
    $$files(components/*.h) \

#TRANSLATIONS += ctrvisual.ts

FORMS += \
#    scenecontrol/providersaveprocessingdialog.ui

LIBS += -L$${ROOT_DIR}bin/lib \
        -lctrcore \
        -lctrrasterrender \
        -lembeddedwidgets \
#        -lGeographic \
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
