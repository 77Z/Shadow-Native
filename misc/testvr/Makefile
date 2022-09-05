CC = g++


all: shadow

shadow: main.cpp
	${CC} -o $@ -I../lib/openvr/headers -L../lib/openvr/lib/linux64 -lopenvr_api -Wl,-rpath=. $^

run: shadow
	./$<