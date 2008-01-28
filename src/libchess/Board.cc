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

#include "Board.hh"
#include <string>


Board::Board(int n,int m) : nlines(n),ncolums(m) {
	board.clear();
	board.insert(board.end(),n,std::vector<Piece*>(m));
}

Board::~Board() {
	board.clear();
}

const std::vector<Piece*>& Board::operator[](int value) const {
	return this->board[value];
}

void Board::createPiece(const Position& pos,Piece *pe) {
	if(typeid(ChessPiece)!=typeid(*pe))
		throw "Invalid Piece";
	delete this->board[pos.posy()][pos.posx()];
	this->board[pos.posy()][pos.posx()]=pe;
}

int Board::getType(const Position& p) const {
	return this->board[p.posy()][p.posx()]->type();
}

int Board::getPieceReal(const Position& p) const {
	return this->board[p.posy()][p.posx()]->pieceReal();
}

int Board::getColor(const Position& p) const {
	return this->board[p.posy()][p.posx()]->color();
}

ChessBoard::ChessBoard(int n, int m) : Board(n,m) {
	for(int i=0;i<n;i++)
		for(int j=0;j<m;j++) {
			board[i][j]=new ChessPiece();
		}
}

ChessBoard::~ChessBoard() {
	for(int i=0;i<nlines;i++)
		for(int j=0;j<ncolums;j++)
			delete board[i][j];
}


void ChessBoard::makeMove(const ChessMove& j)
{
	int tox=j.getto().posx(),toy=j.getto().posy();
	int fromx=j.getfrom().posx(),fromy=j.getfrom().posy();
	delete this->board[toy][tox];
	this->board[toy][tox] = board[fromy][fromx];
	this->board[fromy][fromx]=new ChessPiece();
}

