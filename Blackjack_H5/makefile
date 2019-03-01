#File that must be compiled
OBJECTS_SERVER = server.o sockets.o
OBJECTS_CLIENT = client.o sockets.o
#Header files
DEPENDS = sockets.h
#Output files
SERVER = server
CLIENT = client

CC = gcc
CFLAGS = -Wall -g -std=gnu99 -pedantic
LDFLAGS = -lm

all: $(SERVER) $(CLIENT)

#Make the server executable
$(SERVER): $(OBJECTS_SERVER)
	$(CC) $^ -o $@ $(LDFLAGS)

#Make the client executable
$(CLIENT): $(OBJECTS_CLIENT)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c $(DEPENDS)
	$(CC) $< -c -o $@ $(CFLAGS)

clean:
	rm -rf *.o $(MAIN) $(MAIN).zip

zip: clean
	zip -r $(MAIN).zip *

.PHONY: clean all zip
