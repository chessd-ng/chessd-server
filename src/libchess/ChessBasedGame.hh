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

#ifndef CHESSBASEDGAME_HH
#define CHESSBASEDGAME_HH

#include "BoardGame.hh" 
#include <vector>

class ChessBasedGame : public BoardGame {
	public:
		enum Color {
			WHITE=0,BLACK=1,UNDEFINED=-1
		};

		ChessBasedGame(int n, int m);

		~ChessBasedGame();

		virtual int numberOfTurns() const;

		virtual int winner() const=0;

		virtual int turn() const=0;

		virtual bool verifyCheckMate() const=0;

		virtual bool verifyDraw() const=0;

		virtual const ChessState& getState() const=0;

		virtual bool verifyAndMakeMove(const std::string &move)=0;

		const std::vector<State*>& getHistory() const;

	protected:
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
