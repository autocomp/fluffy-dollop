include(../typeplugin.pri)
ROOT_DIR=../../../
include($${ROOT_DIR}pri/plugins.pri)

# lib
QT += xml

# target
TARGET   = files_translator_webdav

LIBS += -L$${INSTALL_PATH_LIB} \
        -lregionbiz -lqwebdavlib

HEADERS += \
    webdav_queue.h \
    webdav_wrapper.h \
    files_translator_webdav.h \
    cache_manager.h \
    upload_manager.h

SOURCES += \
    webdav_queue.cpp \
    webdav_wrapper.cpp \
    files_translator_webdav.cpp \
    cache_manager.cpp \
    upload_manager.cpp
