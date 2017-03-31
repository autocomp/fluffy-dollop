TARGET=embeddedwidgets

ROOT_DIR = ../

TEMPLATE = lib

CONFIG += plugin

include($${ROOT_DIR}/pri/common.pri)

QMAKE_CXXFLAGS += -std=c++11
QT += sql network

lessThan(QT_MAJOR_VERSION, 5) {
QT += webkit
}

greaterThan(QT_MAJOR_VERSION, 4) {
QT += core widgets
DEFINES += QT_V5
}

TRANSLATIONS += ts/src_ru.ts

SOURCES += \
    embeddedwindow.cpp \
    embeddedstruct.cpp \
    embeddedwidget.cpp \
    embeddedpanel.cpp \
    embeddedwindowheader.cpp \
    embeddedgroupwidget.cpp \
    embeddedwidgetheader.cpp \
    embeddedheader.cpp \
    embeddedgroupheader.cpp \
    embeddedmainwindowheader.cpp \
    embeddedmainwindow.cpp \
    stylesheetdialog.cpp \
    embeddedapp.cpp \
    embeddedsettingswidget.cpp \
    embeddedmidget.cpp \
    embeddedprivate.cpp \
    embeddedsubiface.cpp \
    runninglabel.cpp \
    loadsessionwidget.cpp \
    savesessionwidget.cpp \
    serializeriface.cpp \
    sqliteserializer.cpp \
    embeddedsessionmanager.cpp \
    embeddedutils.cpp \
    curtaillabel.cpp \
    embeddednetworkmanager.cpp \
    dataserializer.cpp \
    embeddednetworktypes.cpp \
    styledebugger.cpp \
#    docviewerwidget.cpp \
#    docdebugger.cpp \
    waitdialog.cpp \
    embeddedlogger.cpp

HEADERS  += \
    embeddedwindow.h \
    embeddedstruct.h \
    embeddedwidget.h \
    embeddedgroupwidget.h \
    embeddedpanel.h \
    embeddedwindowheader.h \
    embeddedwidgetheader.h \
    embeddedheader.h \
    embeddedgroupheader.h \
    embeddedmainwindowheader.h \
    embeddedmainwindow.h \
    stylesheetdialog.h \
    version.h \
    embeddedapp.h \
    embeddedsettingswidget.h \
    embeddedmidget.h \
    embeddedprivate.h \
    embeddedsubiface.h \
    runninglabel.h \
    version.h \
    loadsessionwidget.h \
    savesessionwidget.h \
    serializeriface.h \
    sqliteserializer.h \
    embeddedsessionmanager.h \
    embeddedutils.h \
    curtaillabel.h \
    embeddednetworkmanager.h \
    embeddednetworktypes.h \
    dataserializer.h \
    styledebugger.h \
#    docdebugger.h \
#    docviewerwidget.h \
    waitdialog.h \
    embeddedlogger.h


DEFINES += MODULE_NAME=\\\"$$TARGET\\\"

include ($${ROOT_DIR}/pri/install.pri)

OTHER_FILES += \
    visconf.sql

RESOURCES += \
    emdeddedwidgetsres.qrc



