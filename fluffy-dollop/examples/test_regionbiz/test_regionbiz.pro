ROOT_DIR = $${PWD}/../../
message( $${ROOT_DIR} )
include( $${ROOT_DIR}/pri/common.pri )

# lib
QT       += sql gui
TEMPLATE = app

# target
TARGET   = test_regionbiz
DESTDIR  = $${INSTALL_PATH_BIN}

# src
SOURCES  += \
    main.cpp

# include
INCLUDEPATH += $${INSTALL_PATH_INCLUDE}

# libs
LIBS += -L$${INSTALL_PATH_LIB} \
        -ldl \
        -lregionbiz

HEADERS += \
    test_reciver.h
