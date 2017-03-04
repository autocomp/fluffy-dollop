include( ../pri/common.pri )

# lib
QT       -= gui
QT       += sql
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
        -lregionbiz
