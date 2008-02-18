/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
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
		/*! \brief Constructor function that iniciates a board given its dimensions and the rest...*/
		Chess();

		/*! \return the winner player*/
		virtual int winner() const ;

		/*! \brief return whose turn it is*/
		virtual int turn() const;

		/*! \brief verify if a player has won*/
		virtual bool verifyCheckMate() const;

		/*! \brief verify if the game ended on a draw*/
		virtual bool verifyDraw() const ;

		//TODO fazer uma virtual dessa funcao
		/*! \return return the current state*/
		const ChessState& getState() const;

		/*! \brief A high-level function that make a move if the given move is correct
		 * \return true if the move is valid
		 * \return false if the move isn't valid
		 * */
		virtual bool verifyAndMakeMove(const std::string &jogada);

	private:
		bool verifyCastle(const ChessMove& j) const;

		bool willBeInCheck(const ChessMove& mv) const;

		/*! \brief Verify is a given move is valid. This is a high level function*/
		bool verifyMove(const ChessMove& j) const;

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
};

#endif
