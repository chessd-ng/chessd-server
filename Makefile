SOURCES = \
		  src/Pairing/Game.cc \
		  src/Pairing/Player.cc \
		  src/Pairing/TourneyPlayers.cc \
		  src/Pairing/Tourney.cc \
		  src/Threads/Pool.cc \
		  src/Threads/Task.cc \
		  src/Threads/Dispatcher.cc \
		  src/Util/IDSet.cc \
		  src/Util/Identifier.cc \
		  src/Util/Timer.cc \
		  src/Util/Log.cc \
		  src/XML/iksutil.cc \
		  src/XML/Xml.cc \
		  src/XMPP/Component.cc \
		  src/XMPP/Disco.cc \
		  src/XMPP/Jid.cc \
		  src/XMPP/Node.cc \
		  src/XMPP/RootNode.cc \
		  src/XMPP/Roster.cc \
		  src/XMPP/Stanza.cc \
		  src/XMPP/StanzaBase.cc \
		  src/XMPP/Muc.cc \
		  src/XMPP/Stream.cc \
		  src/libchess/History.cc \
		  src/libchess/ChessMove.cc \
		  src/libchess/State.cc \
		  src/libchess/Chess.cc \
		  src/libchess/Board.cc \
		  src/libchess/Piece.cc \
		  src/libchess/ChessBasedGame.cc \
		  src/MatchManager.cc \
		  src/ChessTourney.cc \
		  src/Core.cc \
		  src/RatingComponent.cc \
		  src/GameChess.cc \
		  src/Game.cc \
		  src/GameRoom.cc \
		  src/MatchChess.cc \
		  src/MatchFactory.cc \
		  src/ComponentBase.cc \
		  src/MatchDatabase.cc \
		  src/RatingDatabase.cc \
		  src/I18n.cc \
		  src/main.cc \
		  src/Agreement.cc \
		  src/DatabaseManager.cc \
		  src/DatabaseInterface.cc \
		  src/GlickoSystem.cc \
		  src/HistoryProcess.cc \
		  src/GameManager.cc \
		  src/AdjournedDatabase.cc \
		  src/GameDatabase.cc \
		  src/AdminComponent.cc
		  src/AdminComponent.cc \
		  src/XMPP/PartialJid.cc


SRCDIR = src
OBJDIR = obj
DEPSDIR = .deps
CXXFLAGS+=-Wall -ggdb3
#CXXFLAGS+=-Wall -O2 -pg -march=native
#CXXFLAGS+=-Wall -O3 -fomit-frame-pointer -march=native -funroll-loops
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
	@echo "Checking denpendecies $<..."
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
