#-------------------------------------------------
#
# Project created by QtCreator 2016-02-02T16:11:08
#
#-------------------------------------------------

TARGET=ctrwidgets

ROOT_DIR=../

TEMPLATE = lib

CONFIG += plugin

include($${ROOT_DIR}/pri/common.pri)

QT += xml
QT += network
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L$${INSTALL_PATH_LIB} \
        -lctrvisual \
#        -ldpf_sfm \
#        -ldpf_io_base \
#        -ldpf_core \
#        -ldpf_geo \
#        -ldpf_math \
#        -ldpf_improc \
#        -ldpf_srv_sfm \
        -lembeddedwidgets

SOURCES += \
#    components/filedialog/*.cpp \
    $$files(components/waitdialog/*.cpp) \
#    components/filepathwidget/*.cpp \
#    components/sourcesensorwidget/*.cpp \
#    components/endworkingwidgets/*.cpp \
#    components/splitters/*.cpp \
#    components/comboboxes/*.cpp

HEADERS += \
#    components/filedialog/*.h \
    $$files(components/waitdialog/*.h) \
#    components/filepathwidget/*.h \
#    components/sourcesensorwidget/*.h \
#    components/endworkingwidgets/*.h \
#    components/splitters/*.h \
#    components/comboboxes/*.h

OTHER_FILES += \

#TRANSLATIONS += ctrwidgets.ts

RESOURCES += \
#    components/sourcesensorwidget/sourcesensorwidget.qrc \
#    ctrwidgets.qrc \
#    components/endworkingwidgets/endworkingwidgets.qrc
    components/waitdialog/resources.qrc

FORMS += \
#    components/filedialog/previewform.ui \
#    components/sourcesensorwidget/sourcesensorwidget.ui

include ($${ROOT_DIR}/pri/install.pri)
