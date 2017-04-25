CFLAGS = -g -Wall

SERVER = simpled
CLIENTS = ssSet ssGet ssDigest ssRun

all: $(SERVER) $(CLIENTS)

csapp.h:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/include/csapp.h

csapp.c:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/src/csapp.c

csapp.o: csapp.h csapp.c

sserver.o: sserver.h sserver.cc

clients: ssSet ssGet ssDigest ssRun

si: simpled.cc csapp.o
	@clear
	@g++ $(CFLAGS) simpled.cc csapp.o -lpthread -o simpled

simpled: simpled.cc csapp.o 
	g++ $(CFLAGS) simpled.cc csapp.o -lpthread -o simpled
	@clear

ssS: ssSet.cc csapp.o sserver.o
	@g++ $(CFLAGS) ssSet.cc sserver.o csapp.o -lpthread -o ssSet
	clear
	@./ssSet localhost 5000 1234 var val

ssSet: ssSet.cc csapp.o sserver.o
	g++ $(CFLAGS) ssSet.cc sserver.o csapp.o -lpthread -o ssSet

ssGet: ssGet.cc csapp.o sserver.o
	g++ $(CFLAGS) ssGet.cc sserver.o csapp.o -lpthread -o ssGet
	@clear

ssDigest: ssDigest.cc csapp.o sserver.o
	g++ $(CFLAGS) ssDigest.cc sserver.o csapp.o -lpthread -o ssDigest

ssRun: ssRun.cc csapp.o sserver.o
	g++ $(CFLAGS) ssRun.cc sserver.o csapp.o -lpthread -o ssRun
	@clear

.PHONY: clean
clean:
	/bin/rm -rf *.o simpled ssSet ssGet ssDigest ssRun
