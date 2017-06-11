include(../typeplugin.pri)
ROOT_DIR=../../../
include($${ROOT_DIR}pri/plugins.pri)

# lib
QT       += gui sql
CONFIG   += no_keywords plugin
TEMPLATE = lib

# target
TARGET   = data_translator_sql

HEADERS += \
    rbp_translator_sql.h \
    rbp_sql_thread.h

SOURCES += \
    rbp_translator_sql.cpp \
    rbp_sql_thread.cpp

LIBS += -L$${INSTALL_PATH_LIB} \
        -lregionbiz
