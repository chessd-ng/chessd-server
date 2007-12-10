#ifndef __CHESSHH__
#define __CHESSHH__

#include "BoardGame.hh"

class Chess : public BoardGame {
	private:
		State atual;
		History historico;

		//These functions do not verify if the move resulted in a check position for the player who is making the move
		/*! \brief Verify if a Horse move is correct*/
		bool verifyHorseMove(const ChessMove& j) const;
		/*! \brief Verify if a Horse move is correct*/
		bool verifyPawnMove(const ChessMove& j) const;
		/*! \brief Verify if a Horse move is correct*/
		bool verifyRookMove(const ChessMove& j) const;
		/*! \brief Verify if a Horse move is correct*/
		bool verifyBishopMove(const ChessMove& j) const;
		/*! \brief Verify if a Horse move is correct*/
		bool verifyQueenMove(const ChessMove& j) const;
		/*! \brief Verify if a Horse move is correct*/
		bool verifyKingMove(const ChessMove& j) const;
		/*! \brief Verify if a Horse move is correct*/
		bool verifyPieceMove(const ChessMove& j) const;
		/*! \brief Verify if a Horse move is correct*/
		bool verifyHook(const ChessMove& j) const;
		/*! \brief Verify if a Horse move is correct*/
		bool verifyEnPassant(const ChessMove& j) const;
		/*! \brief Verify if a Horse move is correct*/

		/*! \brief Verify is a given move is valid. This is a high level function*/
		bool verifyMove(const ChessMove& j) const;

		/*! \brief Verify if a given position is being Atacked by 
		 * another piece of the opposite player */
		bool beingAttacked(const Position &onde,int jogador) const;
		/*! \brief Finds where the king of the given player is*/
		Position findKing(int jogador) const;

		/*! \brief Verify if the given player is in Check*/
		bool verifyCheck(int player) const;
		/*! \brief Verify if the Game is Draw*/
		bool verifyDraw(int player) const;
		/*! \brief Verify if the player given lost*/
		bool verifyCheckMate(int player) const;

		/*! \brief make a given move and does not verify anything, just makes the move*/
		void makeMove(const ChessMove &j) const; //FIXME this const is not good

		/*! \brief Considering the given move is valid, this function
		 * updates the state, history, board and turn */
		void updateMove(const ChessMove &j);

		/*! \brief update The State, considering one move was made*/
		//FIXME sera que tah bom esse comeu??
		void updateState(const ChessMove& j,bool comeu);
		/*! \brief puts the new State on history*/
		void updateHistory();

		//TODO
		//ChessMove PGNtoChessMove(std::string);

		//All the positions returned by this functions are correct (tested by verifyMove)
		std::vector <Position> *getPositions(const Position& p) const;

		//all possible position but some of them may be incolorrect (not tested by verifyMove)
		std::vector <Position> *getpossiblePositions(const Position& p) const;
		std::vector <Position> *getVerticalandHorizontal(const Position& p) const;
		std::vector <Position> *getDiagonals(const Position& p) const;
		virtual void putPieces();
		virtual void updateTurn();
	protected:
#ifdef TESTE
		virtual void hist() const {historico.Desenha();}
#endif
	public:
		color vez;

		/*! \brief verify if a player has won*/
		virtual bool verifyCheckMate() const;
		/*! \brief verify if the game ended on a draw*/
		virtual bool verifyDraw() const {return verifyDraw(0)==true?true:verifyDraw(1);}
		/*! \return the winner player*/
		virtual int Winner() const ;

		/*! \brief return whose turn it is*/
		color Turn() {
			return vez;
		}
		/*! \brief Constructor function that iniciates a board given its dimensions and the rest...*/
		Chess() : BoardGame(8,8) {
			putPieces();
			atual=State();
			historico=History(this->atual);
			vez=BRANCAS;
		}
#ifdef TESTE
		void Desenha() const {
			std::cout << this->atual.getentireFEN() << std::endl;
			gameboard->desenha();
		}
#endif
		//TODO fazer uma virtual dessa funcao
		/*! \return return the current state*/
		const State& getState() const;
		/*! \brief A high-level function that make a move if the given move is correct
		 * \return true if the move is valid
		 * \return false if the move isn't valid
		 * */
		virtual bool verifyandmakeMove(const std::string &jogada);

};

#endif
