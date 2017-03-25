include(../typeplugin.pri)
ROOT_DIR=../../../
include($${ROOT_DIR}pri/plugins.pri)

# lib
QT       += sql gui
CONFIG   += no_keywords plugin
TEMPLATE = lib

# target
TARGET   = xlsx_translator

HEADERS += \
    rbp_xlsx_translator.h

SOURCES += \
    rbp_xlsx_translator.cpp

LIBS += -L$${INSTALL_PATH_LIB} \
        -lregionbiz -lQtXlsx
