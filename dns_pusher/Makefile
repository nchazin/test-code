CC=c++
LIBRARIES=-lresolv -lpthread
FLAGS=-std=c++11

all: dns_pusher

dns_pusher:	dns_pusher.cc
		$(CC) $(FLAGS) -o dns_pusher $(LIBRARIES) dns_pusher.cc -Wl,-rpath,/usr/local/lib

