OBJECTS = main.o pool.o task.o stream.o xml.o component.o jid.o stanza.o jabbernode.o disco.o muc.o stream_listener.o dispatcher.o match.o timer.o pairing/Game.o pairing/Player.o pairing/Tourney.o pairing/TourneyPlayers.o iksutil.o


CXXFLAGS=-Wall -ggdb3 `pkg-config sigc++-2.0 --cflags`
LIBS=-lrt -lpthread -liksemel `pkg-config sigc++-2.0 --libs`
TARGET=chessd
CC=gcc
CXX=g++

all: ${TARGET}
	@echo "done"

${TARGET}: ${OBJECTS}
	${CXX} -o ${TARGET} ${OBJECTS} ${CXXFLAGS} ${LIBS}

xml: xml.o teste_xml.o xml.hh iksutil.o
	${CXX} -o teste_xml xml.o teste_xml.o iksutil.o ${CXXFLAGS} ${LIBS}


clean:
	rm -f ${OBJECTS} ${TARGET}
