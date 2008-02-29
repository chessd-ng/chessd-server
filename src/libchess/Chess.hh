/*
 *   Copyright (c) 2007-2008 C3SL.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

#ifndef CHESS_HH
#define CHESS_HH

#include "ChessBasedGame.hh"

class Chess : public ChessBasedGame {
	public:
		/*! \brief Constructor function that iniciates an Internation Chess game */
		Chess();

		/*! \brief Constructor function that iniciates an Internation Chess game, given the FEN*/
		Chess(const std::string& FEN);

		/*! \return the winner player*/
		virtual int winner() const ;

		/*! \brief return whose turn it is*/
		virtual int turn() const;

		/*! \brief A high-level function that make a move if the given move is correct
		 * \return true if the move is valid
		 * \return false if the move isn't valid
		 * */
		virtual bool verifyAndMakeMove(const std::string &mv);

		/*! \brief This functions consider the move is valid,
		 * does not check anything, just make the move specified
		 * by "mv"
		*/
		void makeMove(const std::string& mv);

		/*! \brief verify if a player has won*/
		virtual bool verifyCheckMate() const;

		/*! \brief verify if the game ended on a draw*/
		virtual int verifyDraw() const ;

		//TODO fazer uma virtual dessa funcao
		/*! \return return the current state*/
		const ChessState& getState() const;
	protected:
		/*! \brief puts the new State on history*/
		void updateHistory();

		virtual void updateTurn();

		/*! \brief Verify if the player given lost*/
		bool verifyCheckMate(int player) const;

	private:
		/*! \brief Verify is a given move is valid. This is a high level function*/
		bool verifyMove(const ChessMove& j) const;

		bool willBeInCheck(const ChessMove& mv) const;

		/*! \brief make a given move and does not verify anything, just makes the move*/
		void makeMove(const ChessMove &j) const; //FIXME this const is not good

		/*! \brief Considering the given move is valid, this function
		 * updates the state, history, board and turn */
		void updateMove(const ChessMove &j);

		/*! \brief update The State, considering one move was made*/
		//FIXME captured shouldn't be here
		void updateState(const ChessMove& j,bool capatured);

		bool verifyCastle(const ChessMove& j) const;

		/*! \brief Verify if the Game is Draw by stalemate*/
		bool verifyStaleMate(int player) const;

		/*! \brief Verify if the Game is Draw by three fold repetirion rule*/
		bool verifyThreefoldRepetition() const;

		/*! \brief Verify if the Game is Draw by impossibility of checkmate*/
		bool verifyImpossibilityOfCheckmate() const;

		//TODO
		//ChessMove PGNtoChessMove(std::string);
		//std::string ChessMovetoPGN(const ChessMove& c);

		virtual void putPieces();

		int _turn;

};

#endif
