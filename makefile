# makefile

all: dataserver simpleclient

reqchannel.o: reqchannel.H reqchannel.C
	g++ -c -g reqchannel.C

dataserver: dataserver.C reqchannel.o 
	g++ -g -o dataserver dataserver.C reqchannel.o -lpthread

simpleclient: BoundedBuffer.C simpleclient.C reqchannel.o
	g++ -g -o simpleclient BoundedBuffer.C simpleclient.C reqchannel.o -lpthread
