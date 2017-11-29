ROOT_DIR=../
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
    rb_biz_relations.cpp \
    rb_metadata.cpp \
    rb_base_entity.cpp \
    rb_marks.cpp \
    rb_signal_manager.cpp \
    rb_entity_filter.cpp \
    rb_files_translator.cpp \
    rb_files.cpp \
    rb_translator.cpp \
    rb_data_translator.cpp \
    rb_meta_constraints.cpp \
    rb_group.cpp \
    rb_layers.cpp \
    rb_transform_matrix.cpp \
    rb_metadata_custom.cpp \
    rb_graph.cpp

HEADERS  += \
    rb_manager.h \
    rb_locations.h \
    rb_biz_relations.h \
    rb_metadata.h \
    rb_base_entity.h \
    rb_marks.h \
    rb_signal_manager.h \
    rb_entity_filter.h \
    rb_files_translator.h \
    rb_files.h \
    rb_plugin_register.h \
    rb_data_translator.h \
    rb_translator.h \
    rb_meta_constraints.h \
    rb_group.h \
    rb_layers.h \
    rb_transform_matrix.h \
    rb_metadata_custom.h \
    rb_graph.h

win32 {
    DEFINES += STACKWALKER
    SOURCES += StackWalker/StackWalker.cpp
    HEADERS += StackWalker/StackWalker.h
}
