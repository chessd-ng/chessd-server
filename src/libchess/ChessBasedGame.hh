#ifndef CHESSBASEDGAME_HH
#define CHESSBASEDGAME_HH

#include "BoardGame.hh" 
#include "State.hh"
#include <vector>

class ChessBasedGame : public BoardGame {
	public:
		enum Color {
			WHITE=0,BLACK=1,UNDEFINED=-1
		};

		ChessBasedGame(int n, int m);

		virtual int numberOfTurns() const;

		virtual int winner() const=0;

		virtual int turn() const=0;

		virtual bool verifyCheckMate() const=0;

		virtual bool verifyDraw() const=0;

		virtual const State& getState() const=0;

		virtual const std::vector<State> &getHistory() const=0;

		virtual bool verifyAndMakeMove(const std::string &jogada)=0;


	protected:
		State current_state;

		History historico;

		/*! \brief Verify if the given player is in Check*/
		bool verifyCheck(int player) const;

		virtual bool verifyMove(const ChessMove& j) const=0;

		void setState(const std::string& FEN) const;

		std::string getPosForFEN() const;

		//All the positions returned by this functions are correct (tested by verifyMove)
		std::vector <Position> *getPositions(const Position& p) const;

		/*! \brief Verify if a given position is being Atacked by 
		 * another piece of the opposite player */
		bool beingAttacked(const Position &onde,int jogador) const;

		/*! \brief Finds where the king of the given player is*/
		Position findKing(int jogador) const;

		/*! \brief Verify if a a Pawn made EnPassant*/
		bool verifyEnPassant(const ChessMove& j) const;

		bool verifyPieceMove(const ChessMove& j) const;


	private:
		bool VerifyDiagonal(const ChessMove& para) const;

		bool VerifyHorizontal(const ChessMove& para) const;

		bool VerifyVertical(const ChessMove& para) const;

		//These functions do not verify if the move resulted in a check position for the player who is making the move
		/*! \brief Verify if a Horse move is correct*/
		bool verifyHorseMove(const ChessMove& j) const;

		/*! \brief Verify if a Pawn move is correct*/
		bool verifyPawnMove(const ChessMove& j) const;

		/*! \brief Verify if a Rook move is correct*/
		bool verifyRookMove(const ChessMove& j) const;

		/*! \brief Verify if a Bishop move is correct*/
		bool verifyBishopMove(const ChessMove& j) const;

		/*! \brief Verify if a Queen move is correct*/
		bool verifyQueenMove(const ChessMove& j) const;

		/*! \brief Verify if a King move is correct*/
		bool verifyKingMove(const ChessMove& j) const;

		//all possible position but some of them may be incorrect (not tested by verifyMove)
		std::vector <Position> *getpossiblePositions(const Position& p) const;

		std::vector <Position> *getVerticalandHorizontal(const Position& p) const;

		std::vector <Position> *getDiagonals(const Position& p) const;
};

#endif
