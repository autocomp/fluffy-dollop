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
    rbp_files_translator_ftp.h \
    rbp_ftp_wrapper.h \
    rbp_file_load_manager.h

SOURCES += \
    rbp_files_translator_ftp.cpp \
    rbp_ftp_wrapper.cpp \
    rbp_file_load_manager.cpp

LIBS += -L$${INSTALL_PATH_LIB} \
        -lregionbiz -lQtFtp
