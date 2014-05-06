EXE=sim

SOURCES=lennard_jones.c atomo_molecula.c ga.c simulador.c main_console.c
#SOURCES=$(wildcard *.c)
HEADERS=simulador.h atomo_molecula.h lennard_jones.h morse.h ga.h
#HEADERS=$(wildcard *.h)

DEBUG=-ggdb -Wall
CC=gcc

LDFLAGS=$(DEBUG)
CFLAGS=$(DEBUG) -c
LIBS += -lm -lgsl -lgslcblas
LIBS += -lm
OBJECTS=$(SOURCES:.c=.o)



all: $(OBJECTS) $(EXE)

$(EXE): $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

#test: $(OBJECTS) test.o $(HEADERS)
#	$(CC) $(LDFLAGS) $(OBJECTS) test.o -o test $(LIBS)


%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o $(EXE)

# For flymake-mode in emacs
check-syntax:
	gcc -Wall -fsyntax-only -S -o /dev/null $(CHK_SOURCES)

