#ifndef GAMESTANDARD_HH
#define GAMESTANDARD_HH

#include "GameChess.hh"

//TODO ver negocios do tempo!
class GameStandard : public GameChess {
	public:
		GameStandard(const StandardPlayerList& _players);

		virtual ~GameStandard() {};

		virtual GameResult* newGameResult(const std::string& endreason, const TeamResultList &l, const std::vector<State> &s) const;

	private:
};

class ChessStandardGameResult : public ChessGameResult {
	public:
		ChessStandardGameResult(const std::string &endreason,const TeamResultList &l,const std::vector<State> &s);

		virtual void updateRating(std::map<Player, Rating> &ratings) const;
	private:
};

#endif
