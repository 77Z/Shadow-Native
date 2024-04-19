CXX = g++

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: AXENativeLink.node

AXENativeLink.node: $(OBJECTS)
	${CXX} -o $@ $^ -shared -fPIC

%.o: %.cpp
	${CXX} -c -o $@ $< -Iheaders -Iheaders/node_headers -Iheaders/node_headers/include -fPIC