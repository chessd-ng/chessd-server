SOURCES = \
		  src/Agreement.cc \
		  src/ComponentBase.cc \
		  src/ComponentWrapper.cc \
		  src/Core.cc \
		  src/CoreInterface.cc \
		  src/Game.cc \
		  src/GameManager.cc \
		  src/GameProtocol.cc \
		  src/GameRoom.cc \
		  src/MatchDatabase.cc \
		  src/MatchManager.cc \
		  src/MatchProtocol.cc \
		  src/Pairing/Game.cc \
		  src/Pairing/Player.cc \
		  src/Pairing/TourneyPlayers.cc \
		  src/Pairing/Tourney.cc \
		  src/Query.cc \
		  src/QueryBase.cc \
		  src/RatingManager.cc \
		  src/StreamListener.cc \
		  src/Threads/Dispatcher.cc \
		  src/Threads/Pool.cc \
		  src/Threads/Task.cc \
		  src/Util/IDSet.cc \
		  src/Util/Identifier.cc \
		  src/Util/Timer.cc \
		  src/XML/Xml.cc \
		  src/XML/iksutil.cc \
		  src/XMPP/Component.cc \
		  src/XMPP/Disco.cc \
		  src/XMPP/Jid.cc \
		  src/XMPP/Muc.cc \
		  src/XMPP/Node.cc \
		  src/XMPP/RootNode.cc \
		  src/XMPP/Roster.cc \
		  src/XMPP/Stanza.cc \
		  src/XMPP/StanzaBase.cc \
		  src/XMPP/Stream.cc \
		  src/main.cc \
		  src/GameChess.cc \
		  src/GameStandard.cc \
		  src/MatchChess.cc \
		  src/MatchStandard.cc \
		  src/libchess/Board.cc \
		  src/libchess/Chess.cc \
		  src/libchess/ChessMove.cc \
		  src/libchess/Piece.cc \
		  src/libchess/State.cc \
		  src/libchess/rules.cc


SRCDIR = src
OBJDIR = obj
DEPSDIR = .deps
CXXFLAGS=-Wall -ggdb3 -D_GLIBCXX_DEBUG
#CXXFLAGS=-Wall -O3 -fomit-frame-pointer -pg -march=native
#CXXFLAGS=-Wall -O3 -fomit-frame-pointer -march=native
LDLIBS=-I${HOME}/.usr/lib -lrt -lpthread -liksemel
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
	${CXX} ${CXXFLAGS} -MM $< | sed 's/^[^:]*:/$(subst /,\/,$(patsubst ${SRCDIR}/%.cc,${OBJDIR}/%.o,$<)) $(subst /,\/,$@):/' > $@

obj/%.o: ${SRCDIR}/%.cc
	@mkdir -p $(dir $@)
	${CXX} -o $@ -c ${CXXFLAGS} $<

clean: clean-target clean-obj

clean-target:
	rm -f ${TARGET}

clean-obj:
	rm -rf ${OBJDIR}

clean-deps:
	rm -rf ${DEPSDIR}
