include(../typeplugin.pri)
ROOT_DIR=../../../
include($${ROOT_DIR}pri/plugins.pri)

# lib
QT       += gui
CONFIG   += no_keywords plugin
TEMPLATE = lib

# target
TARGET   = files_translator_ftp

HEADERS += \
    rbp_files_translator_ftp.h

SOURCES += \
    rbp_files_translator_ftp.cpp

LIBS += -L$${INSTALL_PATH_LIB} \
        -lregionbiz -lQtFtp
