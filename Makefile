
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic
INCLUDE = -I /usr/local/include

all: libcmpp2.so

libcmpp2.so: src/command.o src/packet.o src/utils.o src/socket.o
	$(CC) $(CFLAGS) $(INCLUDE) -shared -fPIC src/command.o src/packet.o src/socket.o src/utils.o -o libcmpp2.so

example: sp ismg deliver

sp: example/sp.c
	$(CC) $(CFLAGS) -g $(INCLUDE) -pthread -lssl -lcrypto -liconv -lcmpp2 example/sp.c -o example/sp

ismg: example/ismg.c
	$(CC) $(CFLAGS) -g $(INCLUDE) -pthread -lssl -lcrypto -liconv -lcmpp2 example/ismg.c -o example/ismg

deliver: example/deliver.c
	$(CC) $(CFLAGS) -g $(INCLUDE) -pthread -lssl -lcrypto -liconv -lcmpp2 example/deliver.c -o example/deliver

src/command.o: src/command.c src/command.h
	$(CC) $(CFLAGS) src/command.c -o src/command.o

src/command.o: src/packet.c src/packet.h
	$(CC) $(CFLAGS) src/packet.c -o src/packet.o

src/command.o: src/socket.c src/socket.h
	$(CC) $(CFLAGS) src/socket.c -o src/socket.o

src/command.o: src/utils.c src/utils.h
	$(CC) $(CFLAGS) src/utils.c -o src/utils.o

.PHONY: install clean

install:
	install -m 0644 src/cmpp.h /usr/include
	install -m 0755 libcmpp2.so /lib64

clean:
	rm -f src/*.o
	rm -f libcmpp2.so
	rm -f example/sp
	rm -f example/ismg
	rm -f example/deliver
