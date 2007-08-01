#ifndef GAME_HISTORY_HH
#define GAME_HISTORY_HH

#include <vector>
#include "game_move.hh"

typedef std::vector<GameMove> GameMoveList;

/* stores the history of a game */

class GameHistory {
	public:
		const GameMoveList& moves() const { return this->_moves; }
		GameMoveList& moves() { return this->_moves; }
		
		const std::string& status() const { return this->_status; }
		std::string& status() { return this->_status; }

	private:
		std::vector<GameMove> _moves;
		/* current status, the game
		 * might not be finished    */
		std::string _status;
};

#endif
