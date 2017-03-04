#defines
ROOT_DIR = $${PWD}/../
INSTALL_PATH_BIN       = $${ROOT_DIR}bin
INSTALL_PATH_LIB       = $${INSTALL_PATH_BIN}/lib
INSTALL_PATH_PLUGINS   = $${INSTALL_PATH_BIN}/plugins/
TRANSLATE_PATH         = $${INSTALL_PATH_BIN}/translate
INSTALL_PATH_INCLUDE   = $${ROOT_DIR}/include \

#share qmake defines
INCLUDEPATH += /usr/include/pgsql/ /usr/local/include
INCLUDEPATH += $${ROOT_DIR} $${INSTALL_PATH_INCLUDE}

BUILD_PATH = $${ROOT_DIR}/build/$$TARGET

OBJECTS_DIR = $${BUILD_PATH}/objects
MOC_DIR     = $${BUILD_PATH}/mocs
UI_DIR      = $${BUILD_PATH}/uics
RCC_DIR     = $${BUILD_PATH}/rcc

LIBS += -L$${INSTALL_PATH_LIB} -L$${ROOT_DIR}lib -L/usr/local/lib

QMAKE_CXXFLAGS += -std=c++11 -Wall
QMAKE_CXXFLAGS += -ffast-math
QMAKE_CXXFLAGS += -D_GLIBCXX_USE_NANOSLEEP # Для spdlog

CONFIG += debug


build_mode = $$find(TEMPLATE, lib)
count(build_mode, 1){
    DESTDIR = $$INSTALL_PATH_LIB

    lib_mode = $$find(CONFIG, dll)
    count(lib_mode, 1){
        DESTDIR = $$INSTALL_PATH_PLUGINS$$TYPEPLUGIN
    }
}

build_mode = $$find(TEMPLATE, app)
count(build_mode, 1){
    DESTDIR = $$INSTALL_PATH_BIN
}

system("echo Installation path: $$TARGET  = $$DESTDIR; echo")


isEmpty(QMAKE_LRELEASE){
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

isEmpty(TRANSLATIONS){
updateqm.input = TRANSLATIONS
updateqm.output = $${TRANSLATE_PATH}/$${TARGET}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm $${TRANSLATE_PATH}/$${TARGET}.qm
updateqm.CONFIG += no_link

QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all
}

DEFINES += PLUGIN_BUILD_NUMBER=\\\"$$(PL_BUILD_NUMBER)\\\"




