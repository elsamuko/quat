
# ui
INCLUDEPATH += $${SRC_DIR}/gui
HEADERS += $$files($${SRC_DIR}/gui/*.h)
SOURCES += $$files($${SRC_DIR}/gui/*.cxx)

# fluid
fluid.output = ${QMAKE_FILE_PATH}$${DIR_SEPARATOR}${QMAKE_FILE_BASE}.cxx
fluid.commands = cd ${QMAKE_FILE_PATH} && fluid -c -o ${QMAKE_FILE_BASE}.cxx -h ${QMAKE_FILE_BASE}.h ${QMAKE_FILE_BASE}.fl
fluid.input = FLUIDS
fluid.variable_out = GENERATED_SOURCES
QMAKE_EXTRA_COMPILERS += fluid

fluid_h.output = ${QMAKE_FILE_PATH}$${DIR_SEPARATOR}${QMAKE_FILE_BASE}.h
fluid_h.commands = cd ${QMAKE_FILE_PATH} && fluid -c -o ${QMAKE_FILE_BASE}.cxx -h ${QMAKE_FILE_BASE}.h ${QMAKE_FILE_BASE}.fl
fluid_h.input = FLUIDS
fluid_h.variable_out = HEADERS
QMAKE_EXTRA_COMPILERS += fluid_h

INCLUDEPATH += $${SRC_DIR}/gui/fl
FLUIDS += $${SRC_DIR}/gui/fl/ViewEditor.fl
FLUIDS *= $$files($${SRC_DIR}/gui/fl/*.fl)
