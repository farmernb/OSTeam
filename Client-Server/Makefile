.PHONY: all
all: CS

CS: client.cc server.cc
        g++ -o server server.cc -lpthread
        g++ -o client client.cc -lpthread

.PHONY: clean
clean:
        rm client server