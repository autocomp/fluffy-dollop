include( ../pri/common.pri )

# lib
QT       += sql gui
CONFIG   += no_keywords plugin
TEMPLATE = lib

# target
TARGET   = regionbiz
DESTDIR  = $${INSTALL_PATH_LIB}

# src
SOURCES  += \
    rb_locations.cpp \
    rb_manager.cpp \
    rb_translator.cpp \
    rb_selection_manager.cpp \
    rb_biz_relations.cpp \
    rb_translator_sql.cpp
HEADERS  += \
    rb_manager.h \
    rb_locations.h \
    rb_translator.h \
    rb_selection_manager.h \
    rb_biz_relations.h \
    rb_translator_sql.h
