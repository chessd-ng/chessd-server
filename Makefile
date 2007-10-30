SOURCES = \
		  src/Threads/Pool.cc \
		  src/Threads/Dispatcher.cc \
		  src/Threads/Task.cc \
		  src/XMPP/Component.cc \
		  src/XMPP/Stream.cc \
		  src/XMPP/Muc.cc \
		  src/XMPP/Node.cc \
		  src/XMPP/Stanza.cc \
		  src/XMPP/Disco.cc \
		  src/XMPP/Jid.cc \
		  src/XMPP/Roster.cc \
		  src/XMPP/RootNode.cc \
		  src/XML/iksutil.cc \
		  src/XML/Xml.cc \
		  src/Core.cc \
		  src/MatchManager.cc \
		  src/GameRoom.cc \
		  src/StreamListener.cc \
		  src/Pairing/Player.cc \
		  src/Pairing/Game.cc \
		  src/Pairing/TourneyPlayers.cc \
		  src/Pairing/Tourney.cc \
		  src/Util/Identifier.cc \
		  src/Util/IDSet.cc \
		  src/Util/Timer.cc \
		  src/GameProtocol.cc \
		  src/main.cc \
		  src/MatchStandard.cc \
		  src/GameManager.cc \
		  src/Game.cc \
		  src/MatchProtocol.cc \
		  src/Agreement.cc \
		  src/CoreInterface.cc \
		  src/ComponentWrapper.cc \
		  src/MatchDatabase.cc

SRCDIR = src
OBJDIR = obj
DEPSDIR = .deps
CXXFLAGS=-Wall -g -D_GLIBCXX_DEBUG
#CXXFLAGS=-Wall -O3 -fomit-frame-pointer -funroll-loops -march=native
LDLIBS=-lrt -lpthread -liksemel
TARGET=chessd
CC=gcc
CXX=g++

OBJECTS = $(patsubst ${SRCDIR}/%.cc,${OBJDIR}/%.o,${SOURCES})
DEPS = $(patsubst ${SRCDIR}/%.cc,${DEPSDIR}/%.d,${SOURCES})

all: ${TARGET}
	@echo "done"

-include ${DEPS}

${TARGET}: ${OBJECTS}
	${CXX} -o ${TARGET} ${OBJECTS} ${CXXFLAGS} ${LDLIBS}

.deps/%.d: ${SRCDIR}/%.cc
	@mkdir -p $(dir $@)
	${CXX} ${CXXFLAGS} -MM $< | sed 's/\(^[^ \.]*\)\.o/${OBJDIR}\/\1.o ${DEPSDIR}\/\1.d/' > $@

obj/%.o: ${SRCDIR}/%.cc
	@mkdir -p $(dir $@)
	${CXX} -o $@ -c ${CXXFLAGS} $<

clean: clean-target clean-obj

clean-target:
	rm -f ${TARGET}

clean-obj:
	rm -rf ${OBJDIR}

clean-depends:
	rm -rf ${DEPSDIR}
