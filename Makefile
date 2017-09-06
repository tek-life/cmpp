
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic
INCLUDE = -I /usr/local/include

all: libcmpp2.so

libcmpp2.so: src/command.c src/command.h src/packet.c src/packet.h src/utils.c src/utils.h src/socket.c src/socket.h
	$(CC) $(CFLAGS) $(INCLUDE) -shared -fPIC src/command.c src/packet.c src/socket.c src/utils.c -o libcmpp2.so

example: sp ismg deliver

sp: example/sp.c
	$(CC) $(CFLAGS) -g $(INCLUDE) -pthread -lssl -lcrypto -liconv -lcmpp2 example/sp.c -o example/sp

ismg: example/ismg.c
	$(CC) $(CFLAGS) -g $(INCLUDE) -pthread -lssl -lcrypto -liconv -lcmpp2 example/ismg.c -o example/ismg

deliver: example/deliver.c
	$(CC) $(CFLAGS) -g $(INCLUDE) -pthread -lssl -lcrypto -liconv -lcmpp2 example/deliver.c -o example/deliver

.PHONY: install clean

install:
	install -m 0644 src/cmpp.h /usr/include
	install -m 0755 libcmpp2.so /usr/lib64

clean:
	rm -f src/*.o
	rm -f libcmpp2.so
	rm -f example/sp
	rm -f example/ismg
	rm -f example/deliver
