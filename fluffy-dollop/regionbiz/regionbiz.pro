include( ../pri/common.pri )

# lib
QT       -= gui
QT       += sql
CONFIG   += plugin
TEMPLATE = lib

# target
TARGET   = regionbiz
DESTDIR  = $${INSTALL_PATH_LIB}

# src
SOURCES  += \
    rb_locations.cpp \
    rb_manager.cpp \
    rb_translator.cpp \
    rb_translator_sqlite.cpp
HEADERS  += \
    rb_manager.h \
    rb_locations.h \
    rb_translator.h \
    rb_translator_sqlite.h
