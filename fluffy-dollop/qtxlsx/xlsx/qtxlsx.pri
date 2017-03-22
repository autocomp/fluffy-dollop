INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += core gui gui-private
#!build_xlsx_lib:DEFINES += XLSX_NO_LIB

HEADERS += \
    *.h

SOURCES += \
    *.cpp

