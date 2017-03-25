TARGET=2Dvisplugin

include(../typeplugin.pri)
ROOT_DIR=../../

include($${ROOT_DIR}pri/plugins.pri)

QT += network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L$${ROOT_DIR}bin/lib \
        -lctrcore \
        -lctrvisual \
        -lctrrasterrender \
#        -lGeographic \
#        -ldpf_core \
#        -ldpf_geo \
#        -ldpf_io_base \
#        -ldpf_srv_immeasure \
#        -lboost_program_options \
#        -lboost_date_time \
#        -lboost_system \
#        -lobjrepr \
#        -lctrvectorrender

SOURCES += \
    *.cpp \

HEADERS += \
    *.h \

DISTFILES += visualizer2d.json

#TRANSLATIONS += libvisualizer2d.ts

RESOURCES += \
    libvisualizer2d.qrc

FORMS += \
    viravisualizer2dform.ui
