ROOT_DIR=../
include( ../pri/common.pri )

# Config
QT = core network
TEMPLATE = lib
CONFIG += plugin

# Target
TARGET = QtFtp
DESTDIR  = $${INSTALL_PATH_LIB}

# Input
HEADERS += qftp.h qurlinfo.h
SOURCES += qftp.cpp qurlinfo.cpp
