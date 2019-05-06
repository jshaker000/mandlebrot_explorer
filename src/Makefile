SHELL         := /bin/bash
SRCDIR        := ${CURDIR}
IDIR          := ${CURDIR}/../include
ODIR          := ${CURDIR}/../obj
INSTALLDIR    := /usr/local/bin
CXX           := g++
CXXFLAGS      := -std=c++11 -O2 -I$(IDIR)
LDFLAGS       := -lSDL2 -lpthread


TARGETS     := mandlebrot_explorer

_DEPS   := config.h rendering.h
DEPS    := $(patsubst %,$(IDIR)/%,$(_DEPS))

_MAINS  := $(addsuffix .o, $(TARGETS) )
MAINS   := $(patsubst %,$(ODIR)/%,$(_MAINS))

_OBJ    := config.o rendering.o $(_MAINS)
OBJ     := $(patsubst %,$(ODIR)/%,$(_OBJ))

CONFIGS_CPP := config.cpp

.PHONY: all install uninstall remove clean

all: $(TARGETS)

install: all
	for i in $(TARGETS); do cp -f $$i $(INSTALLDIR)/$$i; done

uninstall:
	for i in $(TARGETS); do rm -f $(INSTALLDIR)/$$i; done

remove:
	rm -f $(TARGETS)

clean:
	rm -f $(ODIR)/*.o

$(SRCDIR)/$(CONFIGS_CPP): % : %.def
	@if [ -f $(SRCDIR)/$@ ]; \
	then \
		printf 'Overwrite $@ with $<?\n'; \
		read -n 1 -p '>' userinput; \
		printf '\n'; \
		if [ "$$userinput" = "y" ] || [ "$$userinput" = "Y" ]; \
			then \
			cp -f $< $@; \
		fi; \
	else \
		cp -f $< $@; \
	fi;

$(OBJ): $(ODIR)/%.o : $(SRCDIR)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGETS): % : $(filter-out $(MAINS), $(OBJ)) $(ODIR)/%.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)