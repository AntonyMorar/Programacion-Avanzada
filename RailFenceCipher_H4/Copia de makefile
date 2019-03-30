OBJECTS = main.o pipe.o rail_fence.o
DEPENDS = pipe.h rail_fence.h
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
