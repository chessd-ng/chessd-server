OBJECTS = main.o Threads/Pool.o Threads/Task.o XMPP/Stream.o XML/Xml.o XMPP/Component.o XMPP/Jid.o XMPP/Stanza.o XMPP/Node.o XMPP/Disco.o ComponentListener.o Threads/Dispatcher.o Util/Timer.o Pairing/Game.o Pairing/Player.o Pairing/Tourney.o Pairing/TourneyPlayers.o XML/iksutil.o Core.o


CXXFLAGS=-Wall -ggdb3
LIBS=-lrt -lpthread -liksemel
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
