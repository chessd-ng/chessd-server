OBJECTS=main.o pool.o task.o stream.o xml.o component.o jid.o stanza.o jabbernode.o disco.o muc.o stream_listener.o dispatcher.o match.o timer.o
CXXFLAGS=-Wall -ggdb3 `pkg-config sigc++-2.0 --cflags`
LIBS=-lrt -lpthread -liksemel `pkg-config sigc++-2.0 --libs`
TARGET=chessd
CC=gcc
CXX=g++

all: ${TARGET}
	@echo "done"

${TARGET}: ${OBJECTS}
	${CXX} -o ${TARGET} ${OBJECTS} ${CXXFLAGS} ${LIBS}

clean:
	rm -f ${OBJECTS} ${TARGET}
