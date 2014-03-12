EXE=simulador

SOURCES=simulador.c lennard_jones.c atomo_molecula.c ga.c misc_functions.c
#SOURCES=$(wildcard *.c)
HEADERS=simulador.h atomo_molecula.h lennard_jones.h ga.h misc_functions.h
#HEADERS=$(wildcard *.h)

DEBUG=-ggdb
CC=gcc

LDFLAGS=$(DEBUG)
CFLAGS=$(DEBUG) -c
LIBS += -lm -lgsl -lblas
OBJECTS=$(SOURCES:.c=.o)



all: $(OBJECTS) $(EXE)

$(EXE): $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o $(EXE)

# For flymake-mode in emacs
check-syntax:
	gcc -Wall -fsyntax-only -S -o /dev/null $(CHK_SOURCES)

