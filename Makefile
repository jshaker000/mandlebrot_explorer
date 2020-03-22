# top level makefile that can recurse into src to build and run the software, or clean
# In this way, you can simple run top level "make" to make the program

MAIN := ./src/./mandlebrot_explorer

.PHONY: default clean install uninstall

run: all
	${MAIN}

all:
	make -C src/ all

clean:
	make -C src/ clean

install:
	make -C src/ install

uninstall:
	make -C src/ uninstall
