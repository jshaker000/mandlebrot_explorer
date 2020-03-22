# top level makefile that can recurse into src to build and run the software, or clean
# In this way, you can simple run top level "make" to make the program

SRC_DIR := ./src/
MAIN    := ${SRC_DIR}/./mandlebrot_explorer

RECURSE_CMDS := all clean install uninstall

.PHONY: run ${RECURSE_CMDS}

run: all
	${MAIN}


${RECURSE_CMDS}: % :
	make -C ${SRC_DIR} $@
