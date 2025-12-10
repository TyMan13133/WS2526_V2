CC=g++

CFLAGS=$(shell pkg-config --cflags opencv)
LIBS=$(shell pkg-config --libs opencv)

OBJS= main.o TASK3.o SHA256.o SIMPLESOCKET.o
DEMOTARGET=main server client

client.o:	client.C
	$(CC) -c $<  -std=c++11

server.o:	server.C
	$(CC) -c $<  -std=c++11

SIMPLESOCKET.o:	SIMPLESOCKET.C
	$(CC) -c $<  -std=c++11

SHA256.o:	SHA256.C
	$(CC) -c $<  -std=c++11

TASK3.o:	TASK3.C
	$(CC) -c $<  -std=c++11

main.o:	main.C
	$(CC) -c $<  -std=c++11



main:	$(OBJS)
	$(CC) -o $@ $^ -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11 -lpthread $(LIBS)


server:	server.o
	$(CC) -o server server.o  SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11

client:	client.o
	$(CC) -o client client.o SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11

clean:
	-rm -r -f   $(DEMOTARGET) *.o DOXYGENDOC  *.txt

doc:
	doxygen Doxyfile


all:	$(DEMOTARGET)
	make clean  && make main && make server && make client

run:	main
	./main

