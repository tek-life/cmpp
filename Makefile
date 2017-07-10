
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic
INCLUDE = -I /usr/local/include

all: libcmpp.so

libcmpp.so: src/cmpp.c src/cmpp.h src/packet.c src/packet.h src/common.c src/common.h src/socket.c src/socket.h
	$(CC) $(CFLAGS) $(INCLUDE) -shared -fPIC src/cmpp.c src/packet.c src/common.c src/socket.c -o libcmpp.so

example: sp ismg deliver

sp: example/sp.c
	$(CC) $(CFLAGS) -g $(INCLUDE) -pthread -lssl -lcrypto -liconv -lcmpp example/sp.c -o example/sp

ismg: example/ismg.c
	$(CC) $(CFLAGS) -g $(INCLUDE) -pthread -lssl -lcrypto -liconv -lcmpp example/ismg.c -o example/ismg

deliver: example/deliver.c
	$(CC) $(CFLAGS) -g $(INCLUDE) -pthread -lssl -lcrypto -liconv -lcmpp example/deliver.c -o example/deliver

.PHONY: install clean

install:
	install -m 0644 src/cmpp.h /usr/include
	install -m 0755 libcmpp.so /usr/lib

clean:
	rm -f libcmpp.so
	rm -f example/sp
	rm -f example/ismg
