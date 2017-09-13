TEMPLATE = lib
TARGET = qwebdavlib

ROOT_DIR=../
include($${ROOT_DIR}pri/common.pri)

QT       += network xml
QT       -= gui

CONFIG += plugin

# Enable DEBUG output with qDebug()
#DEFINES += DEBUG_WEBDAV

# Enable extended WebDAV properties (see QWebDavItem.h/cpp)
#DEFINES += QWEBDAVITEM_EXTENDED_PROPERTIES

# DO NOT REMOVE - REQUIRED BY qwebdav_global.h
DEFINES += QWEBDAV_LIBRARY

SOURCES += qwebdav.cpp \
    qwebdavitem.cpp \
    qwebdavdirparser.cpp \
    qnaturalsort.cpp

HEADERS += qwebdav.h \
    qwebdavitem.h \
    qwebdavdirparser.h \
    qnaturalsort.h \
    qwebdav_global.h

