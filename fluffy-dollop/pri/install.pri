INSTALL_INCLUDE_DIR=$${ROOT_DIR}/install

QMAKE_EXTRA_TARGETS += headers
headers.commands = @mkdir -p $${INSTALL_INCLUDE_DIR}/$$TARGET; cp --parents $$HEADERS $${INSTALL_INCLUDE_DIR}/$$TARGET






