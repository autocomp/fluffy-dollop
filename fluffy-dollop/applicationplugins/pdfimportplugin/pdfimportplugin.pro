TARGET=pdfimport

include(../typeplugin.pri)
ROOT_DIR=../../

include($${ROOT_DIR}pri/plugins.pri)

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11

DEFINES += EXTERNAL_USING

#LIBS += -L$${ROOT_DIR}bin/lib \
LIBS += -L$${INSTALL_PATH_LIB} \
        -lctrcore \
        -lctrvisual \
        -lctrwidgets \
        -lgdal \
        -lpoppler-qt4 \

HEADERS += \
    pdfimportplugin.h \
    scenepdfimportwidget.h \
    pdfimporterpanel.h \
    pdfeditorform.h \
    pdfpageslistwidget.h \
    pdfeditorview.h

SOURCES += \
    pdfimportplugin.cpp \
    scenepdfimportwidget.cpp \
    pdfimporterpanel.cpp \
    pdfeditorform.cpp \
    pdfpageslistwidget.cpp \
    pdfeditorview.cpp

RESOURCES += \
    pdfimportpluginres.qrc

FORMS += \
    pdfeditorform.ui


