#ifndef GAMECHESS_HH
#define GAMECHESS_HH

#include "Game.hh"
//#include "Chesslib.hh"
#include "Util/Timer.hh"
#include "libchess/Chess.hh"
#include "XMPP/Jid.hh"
#include <vector>


//TODO ver negocios do tempo!
class GameChess : public Game {
	public:
		//FIXME ver se a passagem pode ser feita por referencia
		GameChess(const StandardPlayerList& _players);

		virtual ~GameChess() {};

		virtual XML::Tag* state() const;

		virtual const std::string& category() const = 0;

		virtual const std::string& title() const;

		/*! \brief The player has resigned */
		virtual void resign(const Player& player);

		/*! \brief The player has called a flag */
		virtual void call_flag(const Player& player);

		/*! \brief The players agreed on a draw */
		virtual void draw();

		virtual void adjourn();

		/*! \brief Has the game ended?
		 *
		 * \return Returns the game result if the game is over, NULL otherwise.
		 */
		virtual GameResult* done() const;

		virtual void move(const Player& player, const std::string& movement);

		virtual const TeamList& teams() const;
	private:
		Chess chess;
		TeamList _teams;
		std::string _title;

		//cuidado com a cor
		color _resign;
		bool _draw;

		Util::Time whitetime;
		//map de player -> cor
		std::map<Player,color> colormap;
		StandardPlayerList _players;
};

class ChessGameResult : public GameResult {
	public:
		ChessGameResult(const std::string & endreason,const TeamResultList &l,const std::string& _category);
		virtual ~ChessGameResult(){};
		virtual const std::string& category() const;
		virtual const std::string& end_reason() const;
		virtual const PlayerList& players() const;
		virtual const TeamResultList& results() const;
		virtual XML::Tag* history() const;
		virtual void updateRating(std::map<Player, Rating>& ratings)const;
	private:
		std::string _end_reason;
		TeamResultList teamresultlist;
		PlayerList playerlist;
		std::string _category;

};

#endif
