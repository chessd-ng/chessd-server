#ifndef __CHESSHH__
#define __CHESSHH__

#include "ChessBasedGame.hh"

class Chess : public ChessBasedGame {
	private:
		State atual;
		History historico;

		/*! \brief Verify is a given move is valid. This is a high level function*/
		bool verifyMove(const ChessMove& j) const=0;

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
		//std::string ChessMovetoPGN(const ChessMove& c);

		virtual void putPieces();

		virtual void updateTurn();

		int _turn;
	public:

		/*! \brief verify if a player has won*/
		virtual bool verifyCheckMate() const;

		/*! \brief verify if the game ended on a draw*/
		virtual bool verifyDraw() const ;

		/*! \return the winner player*/
		virtual int Winner() const ;

		/*! \brief return whose turn it is*/
		int Turn() const;

		/*! \brief Constructor function that iniciates a board given its dimensions and the rest...*/
		Chess() : ChessBasedGame(8,8) {
			putPieces();
			atual=State();
			historico=History(this->atual);
			_turn=WHITE;
		}

		//TODO fazer uma virtual dessa funcao
		/*! \return return the current state*/
		const State& getState() const;

		/*! \return return a vector of States*/
		const std::vector<State> &getHistory() const;

		/*! \brief A high-level function that make a move if the given move is correct
		 * \return true if the move is valid
		 * \return false if the move isn't valid
		 * */
		virtual bool verifyandmakeMove(const std::string &jogada);

};

#endif
