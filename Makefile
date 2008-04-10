SOURCES = \
		  src/AdminComponent.cc \
		  src/Agreement.cc \
		  src/ChessTourney.cc \
		  src/ComponentBase.cc \
		  src/Core.cc \
		  src/DatabaseInterface.cc \
		  src/DatabaseManager.cc \
		  src/Game.cc \
		  src/GameChess.cc \
		  src/GameManager.cc \
		  src/GameRoom.cc \
		  src/GlickoSystem.cc \
		  src/HistoryProcess.cc \
		  src/I18n.cc \
		  src/libchess/Board.cc \
		  src/libchess/ChessBasedGame.cc \
		  src/libchess/Chess.cc \
		  src/libchess/ChessMove.cc \
		  src/libchess/History.cc \
		  src/libchess/Piece.cc \
		  src/libchess/State.cc \
		  src/main.cc \
		  src/MatchChess.cc \
		  src/MatchDatabase.cc \
		  src/MatchManager.cc \
		  src/Pairing/Game.cc \
		  src/Pairing/Player.cc \
		  src/Pairing/Tourney.cc \
		  src/Pairing/TourneyPlayers.cc \
		  src/RatingComponent.cc \
		  src/Threads/Dispatcher.cc \
		  src/Threads/Pool.cc \
		  src/Threads/Task.cc \
		  src/Util/Identifier.cc \
		  src/Util/IDSet.cc \
		  src/Util/Log.cc \
		  src/Util/Timer.cc \
		  src/XML/iksutil.cc \
		  src/XML/Xml.cc \
		  src/XMPP/Component.cc \
		  src/XMPP/Disco.cc \
		  src/XMPP/Jid.cc \
		  src/XMPP/Muc.cc \
		  src/XMPP/Node.cc \
		  src/XMPP/PartialJid.cc \
		  src/XMPP/RootNode.cc \
		  src/XMPP/Roster.cc \
		  src/XMPP/StanzaBase.cc \
		  src/XMPP/Stanza.cc \
		  src/XMPP/Stream.cc \
		  src/MatchFactory.cc \
		  src/Util/Socket.cc


SRCDIR = src
OBJDIR = obj
DEPSDIR = .deps
#CXXFLAGS+=-Wall -ggdb3
#CXXFLAGS+=-Wall -O2 -pg -march=native
CXXFLAGS+=-Wall -O3 -fomit-frame-pointer -march=native -funroll-loops
CXXFLAGS+=-I${SRCDIR} `pkg-config --cflags iksemel` `pkg-config --cflags libpqxx`
LDLIBS+=-lrt -lpthread `pkg-config --libs iksemel` `pkg-config --libs libpqxx`
TARGET=chessd
CC=gcc
CXX=g++

OBJECTS = $(patsubst ${SRCDIR}/%.cc,${OBJDIR}/%.o,${SOURCES})
DEPS = $(patsubst ${SRCDIR}/%.cc,${DEPSDIR}/%.d,${SOURCES})

all: ${TARGET}
	@echo "done"

-include ${DEPS}

${TARGET}: ${OBJECTS}
	@echo "Linking $@..."
	@${CXX} -o ${TARGET} ${OBJECTS} ${CXXFLAGS} ${LDLIBS}

.deps/%.d: ${SRCDIR}/%.cc
	@echo "Checking denpendencies $<..."
	@mkdir -p $(dir $@)
	@${CXX} ${CXXFLAGS} -MM $< | sed 's/^[^:]*:/$(subst /,\/,$(patsubst ${SRCDIR}/%.cc,${OBJDIR}/%.o,$<)) $(subst /,\/,$@):/' > $@

obj/%.o: ${SRCDIR}/%.cc
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	@${CXX} -c ${CXXFLAGS} -o $@ $<

clean: clean-target clean-obj

clean-target:
	@echo "Cleaning executable..."
	@rm -f ${TARGET}

clean-obj:
	@echo "Cleaning objects..."
	@rm -f ${OBJECTS}

clean-deps:
	@echo "Cleaning dependencies..."
	@rm -rf ${DEPSDIR}
