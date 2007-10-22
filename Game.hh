#ifndef GAME_HH
#define GAME_HH

#include <map>
#include <string>
#include <vector>
#include "Team.hh"
#include "Rating.hh"
#include "XML/Xml.hh"

/*typedef std::map<std::string, std::string> AttributeMap;

typedef AttributeMap PlayerState;
typedef std::vector<PlayerState> PlayerStateList;

class BoardState {
	public:
		BoardState() { }

		const AttributeMap& attributes() const { return this->_attributes; }
		AttributeMap& attributes() { return this->_attributes; }

		const PlayerStateList& players() const { return this->_players; }
		PlayerStateList& players() { return this->_players; }

		void swap(BoardState& b) { 
			std::swap(this->attributes(), b.attributes());
			std::swap(this->players(), b.players());
		}
	private:
		AttributeMap _attributes;
		PlayerStateList _players;
};

typedef std::vector<BoardState> BoardStateList;

class GameState {
	public:
		GameState() { }

		const AttributeMap& attributes() const { return this->_attributes; }
		AttributeMap& attributes() { return this->_attributes; }

		const BoardStateList& boards() const { return this->_boards; }
		BoardStateList& boards() { return this->_boards; }

		void swap(GameState& g) { 
			std::swap(this->attributes(), g.attributes());
			std::swap(this->boards(), g.boards());
		}
	private:
		AttributeMap _attributes;
		BoardStateList _boards;
};
*/

/* Swap overloads */
/*namespace std {
	inline void swap(BoardState& b1, BoardState& b2) {
		b1.swap(b2);
	}
	inline void swap(GameState& g1, GameState& g2) {
		g1.swap(g2);
	}
}
*/

enum TeamResult {
	WINNER,
	DRAWER,
	LOSER,
	UNDEFINED
};

typedef std::vector<std::pair<Team, TeamResult> > TeamResultList;

class GameResult {
	public:
		virtual ~GameResult() { }
		
		/*! \brief The game category */
		virtual const std::string& category() const = 0;

		/*! \brief The reason why the game ended */
		virtual const std::string& end_reason() const = 0;

		/*! \brief List of all players in the game */
		virtual const PlayerList& players() const = 0;

		virtual const TeamResultList& results() const = 0;

		/*! \brief The game history */
		virtual XML::Tag* history() const = 0;

		/*! \brief Update the player ratings
		 *
		 * The given ratings must contain all players in the game.
		 * The map is modified to match new ratings
		 * */
		virtual void updateRating(std::map<Player, Rating>& ratings) const = 0;
};

//typedef XML::Tag GameState;

class Game {
	public:
		virtual ~Game() { }

		/*! \brief Current state of the game */
		virtual XML::Tag* state() const = 0;

		/*! \brief The game category */
		virtual const std::string& category() const = 0;

		/*! \brief A title for the game */
		virtual const std::string& title() const = 0;

		/*! \brief The player has resigned */
		virtual void resign(const Player& player) = 0;

		/*! \brief The player has called a flag */
		virtual void call_flag(const Player& player) = 0;

		/*! \brief The players agreed on a draw */
		virtual void draw() = 0;

		/*! \brief The players agreed on a draw */
		virtual void adjourn() = 0;

		/*! \brief Has the game ended?
		 *
		 * \return Returns the game result if the game is over, NULL otherwise.
		 */
		virtual GameResult* done() const = 0;
		
		/*! \brief Make a move in the game
		 *
		 * \param movement is the description of the movement*/
		virtual void move(const Player& player, const std::string& movement) = 0;

		virtual const TeamList& teams() const = 0;
};



#endif
