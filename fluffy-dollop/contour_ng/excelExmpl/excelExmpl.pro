TARGET = excelExmpl

ROOT_DIR = ../../

TEMPLATE = app

include($${ROOT_DIR}/pri/common.pri)

LIBS += -L$${INSTALL_PATH_LIB} \
        -lQtXlsx \

CONFIG   += console
CONFIG   -= app_bundle

SOURCES += main.cpp
