.PHONY : all clean install uninstall

prefix ?= /usr/local
exec_prefix ?= $(prefix)
bindir ?= $(exec_prefix)/bin

PRE = ds1077l

COMMON_OBJ = ${PRE}.o
COMMON_SRC = ${PRE}.h ${PRE}.c

BUS_PRE = ${PRE}-bus
BUS_BIN = ${BUS_PRE}
BUS_OBJ = ${BUS_PRE}.o
BUS_SRC = ${BUS_PRE}.c ${BUS_PRE}.h ${PRE}.h
BUS_TGT = ${bindir}/${BUS_BIN}

DIV_PRE = ${PRE}-div
DIV_BIN = ${DIV_PRE}
DIV_OBJ = ${DIV_PRE}.o
DIV_SRC = ${DIV_PRE}.c ${DIV_PRE}.h
DIV_TGT = ${bindir}/${DIV_BIN}

MUX_PRE = ${PRE}-mux
MUX_BIN = ${MUX_PRE}
MUX_OBJ = ${MUX_PRE}.o
MUX_SRC = ${MUX_PRE}.c ${MUX_PRE}.h
MUX_TGT = ${bindir}/${MUX_BIN}

WRITEE2_PRE = ${PRE}-writee2
WRITEE2_BIN = ${WRITEE2_PRE}
WRITEE2_OBJ = ${WRITEE2_PRE}.o
WRITEE2_SRC = ${WRITEE2_PRE}.c ${WRITEE2_PRE}.h ${PRE}.h
WRITEE2_TGT = ${bindir}/${WRITEE2_PRE}

BINS = ${BUS_BIN} ${DIV_BIN} ${MUX_BIN} ${WRITEE2_BIN}
INSTALLS = ${BUS_TGT} ${DIV_TGT} ${MUX_TGT} ${WRITEE2_TGT}
OBJS = ${COMMON_OBJ} ${BUS_OBJ} ${DIV_OBJ} ${MUX_OBJ} ${WRITEE2_OBJ}

all : ${BINS}
clean :
	rm -f ${BINS} ${OBJS}
install : ${INSTALLS}
uninstall :
	rm -f ${INSTALLS}

${COMMON_OBJ} : ${COMMON_SRC}

${BUS_OBJ} : ${BUS_SRC}
${BUS_BIN} : ${COMMON_OBJ} ${BUS_OBJ}
${BUS_TGT} : ${BUS_BIN}
	install -m 0755 $^ $@

${DIV_OBJ} : ${DIV_SRC}
${DIV_BIN} : ${COMMON_OBJ} ${DIV_OBJ}
${DIV_TGT} : ${DIV_BIN}
	install -m 0755 $^ $@

${MUX_OBJ} : ${MUX_SRC}
${MUX_BIN} : ${COMMON_OBJ} ${MUX_OBJ}
${MUX_TGT} : ${MUX_BIN}
	install -m 0755 $^ $@

${WRITEE2_OBJ} : ${WRITEE2_SRC}
${WRITEE2_BIN} : ${COMMON_OBJ} ${WRITEE2_OBJ}
${WRITEE2_TGT} : ${WRITEE2_BIN}
	install -m 0755 $^ $@
