.PHONY : all clean
PREFIX=ds1077l

BUSTEST_PRE=${PREFIX}-bus_test
BUSTEST_BIN=${BUSTEST_PRE}
BUSTEST_SRC=${BUSTEST_PRE}.c

DIVTEST_PRE=${PREFIX}-div_test
DIVTEST_BIN=${DIVTEST_PRE}
DIVTEST_SRC=${DIVTEST_PRE}.c

MUXTEST_PRE=${PREFIX}-mux_test
MUXTEST_BIN=${MUXTEST_PRE}
MUXTEST_SRC=${MUXTEST_PRE}.c

BINS=${BUSTEST_BIN} ${DIVTEST_BIN} ${MUXTEST_BIN}

all: ${BINS}
clean:
	rm -rf ${BINS}
