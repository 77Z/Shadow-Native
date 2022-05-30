CC = g++

all: storage

storage: main.cpp
	$(CC) -o $@ $^ -Wl,--format=binary -Wl,data.txt -Wl,--format=default
