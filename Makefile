SOURCES = \
		  Threads/Pool.cc \
		  Threads/Dispatcher.cc \
		  Threads/Task.cc \
		  XMPP/Component.cc \
		  XMPP/Stream.cc \
		  XMPP/Muc.cc \
		  XMPP/Node.cc \
		  XMPP/Stanza.cc \
		  XMPP/Disco.cc \
		  XMPP/Jid.cc \
		  XMPP/Roster.cc \
		  XMPP/RootNode.cc \
		  XML/iksutil.cc \
		  XML/Xml.cc \
		  Core.cc \
		  MatchManager.cc \
		  GameRoom.cc \
		  StreamListener.cc \
		  Pairing/Player.cc \
		  Pairing/Game.cc \
		  Pairing/TourneyPlayers.cc \
		  Pairing/Tourney.cc \
		  Util/Identifier.cc \
		  Util/IDSet.cc \
		  Util/Timer.cc \
		  GameProtocol.cc \
		  main.cc \
		  MatchStandard.cc \
		  GameManager.cc \
		  Game.cc \
		  MatchProtocol.cc \
		  Agreement.cc \
		  CoreInterface.cc \
		  ComponentWrapper.cc \
		  MatchDatabase.cc

OBJDIR = obj
DEPSDIR = .deps
OBJECTS = $(patsubst %.cc,${OBJDIR}/%.o,${SOURCES})
DEPS = $(patsubst %.cc,${DEPSDIR}/%.d,${SOURCES})
#DIRS = $(sort $(dir ${OBJECTS})) $(sort $(dir ${DEPS}))


CXXFLAGS=-Wall -g -D_GLIBCXX_DEBUG
#CXXFLAGS=-Wall -O3 -fomit-frame-pointer -funroll-loops -march=native
LDLIBS=-lrt -lpthread -liksemel
TARGET=chessd
CC=gcc
CXX=g++

all: ${TARGET}
	@echo "done"

-include ${DEPS}

${TARGET}: ${OBJECTS}
	${CXX} -o ${TARGET} ${OBJECTS} ${CXXFLAGS} ${LDLIBS}

.deps/%.d: %.cc
	@mkdir -p $(dir $@)
	${CXX} ${CXXFLAGS} -MM $< | sed 's/\(^[^ \.]*\)\.o/${OBJDIR}\/\1.o ${DEPSDIR}\/\1.d/' > $@

obj/%.o: %.cc
	@mkdir -p $(dir $@)
	${CXX} -o $@ -c ${CXXFLAGS} $<

clean: clean-target clean-obj

clean-target:
	rm -f ${TARGET}

clean-obj:
	rm -rf ${OBJDIR}

clean-depends:
	rm -rf ${DEPSDIR}
