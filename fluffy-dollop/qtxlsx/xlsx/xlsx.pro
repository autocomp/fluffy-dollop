TARGET = QtXlsx

ROOT_DIR = ../../

TEMPLATE = lib

include($${ROOT_DIR}/pri/common.pri)

QMAKE_CXXFLAGS += -std=c++11

QMAKE_DOCS = $$PWD/doc/qtxlsx.qdocconf

CONFIG += build_xlsx_lib plugin

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += core gui gui-private

HEADERS += \
    *.h

SOURCES += \
    *.cpp



