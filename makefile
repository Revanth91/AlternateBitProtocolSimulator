CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I lib/cadmium/include

all: main.o ../build/message.o 
	$(CC) -g -o bin/ABP build/main_top_model.o build/message.o
	$(CC) -g -o bin/SUBNET_TEST build/main_subnet.o build/message.o 
	$(CC) -g -o bin/SENDER_TEST build/main_sender.o build/message.o 
	$(CC) -g -o bin/RECEIVER_TEST build/main_receiver.o build/message.o
	
main.o: src/top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) src/top_model/main.cpp -o build/main_top_model.o
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) test/src/subnet/main.cpp -o build/main_subnet.o
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) test/src/sender/main.cpp -o build/main_sender.o
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) test/src/receiver/main.cpp -o build/main_receiver.o

../build/message.o: 
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) src/data_structures/message.cpp -o build/message.o

clean:
	rm -f ABP build/*.o bin/*
	
