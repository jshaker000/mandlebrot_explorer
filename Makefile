# top level makefile that can recurse into src to build and run the software, or clean
# In this way, you can simple run top level "make" to make the program

MAIN := ./src/./mandlebrot_explorer

.PHONY: default clean

default:
	make -C src/ all
	${MAIN}

clean:
	make -C src/ clean
