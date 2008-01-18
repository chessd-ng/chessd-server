SOURCES = \
		  src/Agreement.cc \
		  src/ComponentBase.cc \
		  src/Core.cc \
		  src/CoreInterface.cc \
		  src/DatabaseManager.cc \
		  src/Game.cc \
		  src/GameChess.cc \
		  src/GameManager.cc \
		  src/GameProtocol.cc \
		  src/GameRoom.cc \
		  src/GameStandard.cc \
		  src/libchess/Board.cc \
		  src/libchess/Chess.cc \
		  src/libchess/ChessMove.cc \
		  src/libchess/Piece.cc \
		  src/libchess/rules.cc \
		  src/libchess/State.cc \
		  src/main.cc \
		  src/MatchChess.cc \
		  src/MatchDatabase.cc \
		  src/MatchManager.cc \
		  src/MatchProtocol.cc \
		  src/MatchStandard.cc \
		  src/Pairing/Game.cc \
		  src/Pairing/Player.cc \
		  src/Pairing/Tourney.cc \
		  src/Pairing/TourneyPlayers.cc \
		  src/QueryBase.cc \
		  src/Query.cc \
		  src/RatingComponent.cc \
		  src/RatingDatabase.cc \
		  src/Threads/Dispatcher.cc \
		  src/Threads/Pool.cc \
		  src/Threads/Task.cc \
		  src/Util/Identifier.cc \
		  src/Util/IDSet.cc \
		  src/Util/Timer.cc \
		  src/XML/iksutil.cc \
		  src/XML/Xml.cc \
		  src/XMPP/Component.cc \
		  src/XMPP/Disco.cc \
		  src/XMPP/Jid.cc \
		  src/XMPP/Muc.cc \
		  src/XMPP/Node.cc \
		  src/XMPP/RootNode.cc \
		  src/XMPP/Roster.cc \
		  src/XMPP/StanzaBase.cc \
		  src/XMPP/Stanza.cc \
		  src/XMPP/Stream.cc \
		  src/GlickoSystem.cc \
		  src/DatabaseInterface.cc \
		  src/GameDatabase.cc


SRCDIR = src
OBJDIR = obj
DEPSDIR = .deps
CXXFLAGS=-Wall -ggdb3 -D_GLIBCXX_DEBUG
#CXXFLAGS=-Wall -O3 -fomit-frame-pointer -pg -march=native
#CXXFLAGS=-Wall -O3 -fomit-frame-pointer -march=native
CXXFLAGS+=-I${SRCDIR} `pkg-config --cflags libpqxx`
LDLIBS=-I${HOME}/.usr/lib -lrt -lpthread -liksemel `pkg-config --libs libpqxx` -lpq
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
	${CXX} -c ${CXXFLAGS} -o $@ $<

clean: clean-target clean-obj

clean-target:
	rm -f ${TARGET}

clean-obj:
	rm -f ${OBJECTS}

clean-deps:
	rm -rf ${DEPSDIR}
