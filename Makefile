EXE=simulador

SOURCES=lennard_jones.c morse.c atomo_molecula.c ga.c main_console.c
#SOURCES=$(wildcard *.c)
HEADERS=simulador.h atomo_molecula.h lennard_jones.h morse.h ga.h main_console.h
#HEADERS=$(wildcard *.h)

DEBUG=-ggdb
CC=gcc

LDFLAGS=$(DEBUG)
CFLAGS=$(DEBUG) -c
#LIBS += -lm -lgsl -lblas
LIBS += -lm
OBJECTS=$(SOURCES:.c=.o)



all: $(OBJECTS) $(EXE)

$(EXE): $(OBJECTS) $(EXE).o $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(EXE).o -o $@ $(LIBS)

test: $(OBJECTS) test.o $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) test.o -o test $(LIBS)


%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o $(EXE)

# For flymake-mode in emacs
check-syntax:
	gcc -Wall -fsyntax-only -S -o /dev/null $(CHK_SOURCES)

