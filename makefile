CFLAGS = -Wall

SERVER = simpled
CLIENTS = ssSet ssGet ssDigest ssRun

all: $(SERVER) $(CLIENTS)

csapp.h:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/include/csapp.h

csapp.c:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/src/csapp.c

csapp.o: csapp.h csapp.c

clients: ssSet ssGet ssDigest ssRun

simpled: simpled.cc csapp.o 
	g++ $(CFLAGS) simpled.cc csapp.o -lpthread -o simpled

ssS: ssSet.cc csapp.o 
	@g++ $(CFLAGS) ssSet.cc csapp.o -lpthread -o ssSet
	clear
	@./ssSet localhost 5000 1234 var val

ssGet: ssGet.cc csapp.o 
	g++ $(CFLAGS) ssGet.cc csapp.o -lpthread -o ssGet

ssDigest: ssDigest.cc csapp.o 
	g++ $(CFLAGS) ssDigest.cc csapp.o -lpthread -o ssDigest

ssRun: ssRun.cc csapp.o 
	g++ $(CFLAGS) ssRun.cc csapp.o -lpthread -o ssRun

.PHONY: clean
clean:
	/bin/rm -rf csapp.h csapp.c *.o simpled ssSet ssGet ssDigest ssRun
