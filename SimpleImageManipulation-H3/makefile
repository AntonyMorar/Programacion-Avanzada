OBJECTS = main.o ppmEditor.o
DEPENDS = ppmEditor.h
MAIN = main

CC = gcc
CFLAGS = -g -std=gnu99 -pedantic

all: $(MAIN)

$(MAIN): $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS)

%.o: %.c $(DEPENDS)
	$(CC) $< -c -o $@ $(CFLAGS)

clean:
	rm -rf *.o $(MAIN) $(MAIN).zip

.PHONY: clean all zip
