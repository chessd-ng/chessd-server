#ifndef __ESTADOH__
#define __ESTADOH__

#include <vector>
#include <string>
#include "Piece.hh"
#include "ChessMove.hh"

struct State {
	std::string tabfen;
	std::string castle;
	Position lastenpassant;
	Position enpassant;
	int halfmoves;
	int fullmoves;
	int vez;

	static std::string toString(int n);

	State();
	State(const std::string posfen);
	State(const State& a);
	std::string getFEN() const;
	std::string getentireFEN() const;
};

class History {
	private:
		std::vector<State> allgame;
	public:
		History();
		History(const State& est);
		~History();
		void putinHistory(const State& est);
		const std::vector<State> &getHistory() const ;
};
#endif
