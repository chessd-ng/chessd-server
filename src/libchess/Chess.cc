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

#include "Chess.hh"
#include "Piece.hh"
#include "ChessMove.hh"
#include <vector>
#include <stdlib.h>

Chess::Chess() : ChessBasedGame(8,8) {
	this->putPieces();
	this->historico->putInHistory(this->current_state);
	this->current_state=new ChessState();
	this->_turn=WHITE;
}

bool Chess::verifyAndMakeMove(const std::string& move) {
	if( move.size()!=4 and move.size()!=5)
		return false;
	if(move[0]<'a' or move[0]>'h')
		return false;
	if(move[1]<'1' or move[1]>'8')
		return false;
	if(move[2]<'a' or move[2]>'h')
		return false;
	if(move[1]<'1' or move[1]>'8')
		return false;
	ChessMove cm(_turn,move);
	if(verifyMove(cm) == true) {
		updateMove(cm);
		return true;
	}
	return false;
}

void Chess::updateTurn() {
	this->_turn=(this->_turn==WHITE?BLACK:WHITE);
}

const ChessState& Chess::getState() const {
	return *(static_cast<ChessState*>(this->current_state));
}

bool Chess::verifyThreefoldRepetition() const {
	int count=0;
	ChessHistory *history=static_cast<ChessHistory*>(this->historico);
	for(int i=0;i<historico->size()-1;i++)
		if((*(*history)[i])==(*(*history)[history->size()-1]))
			count++;
	if(count>=2)
		return true;
	return false;
}

bool Chess::verifyImpossibilityOfCheckmate() const {
	std::vector<std::pair<ChessPiece,Position> > aux[2];
	for(int i=0;i<this->nlines;i++)
		for(int j=0;j<this->ncolums;j++) {
			if((*this->gameboard)[i][j]->type() != ChessPiece::NOTYPE)
				aux[(*this->gameboard)[i][j]->color()].push_back(std::make_pair(*static_cast<ChessPiece*>((*this->gameboard)[i][j]),Position(j,i)));
		}

	if(aux[0].size()==1 and aux[1].size()==1)
		return true;
	else if(aux[0].size()==1 and aux[1].size()==2) {
		for(int i=0;i<(int)aux[1].size();i++)
			if(aux[1][i].first.type()==ChessPiece::BISHOP or aux[1][i].first.type()==ChessPiece::KNIGHT)
				return true;
	}

	else if(aux[0].size()==2 and aux[1].size()==1) {
		for(int i=0;i<(int)aux[0].size();i++)
			if(aux[0][i].first.type()==ChessPiece::BISHOP or aux[0][i].first.type()==ChessPiece::KNIGHT)
				return true;
	}

	else if(aux[0].size()==2 and aux[1].size()==2) {
		for(int i=0;i<(int)aux[0].size();i++)
			if(aux[0][i].first.type()==ChessPiece::BISHOP)
				for(int j=0;j<(int)aux[1].size();j++)
					if(aux[1][j].first.type()==ChessPiece::BISHOP)
						if((aux[0][i].second.x+aux[0][i].second.y)%2 == (aux[1][j].second.x+aux[1][j].second.y)%2)
							return true;
	}
	return false;
}

bool Chess::verifyDraw() const {
	if(verifyThreefoldRepetition()==true)
		return true;

	if(verifyImpossibilityOfCheckmate()==true)
		return true;

	return verifyDraw(0)==true?true:verifyDraw(1);
}

int Chess::turn() const {
	return _turn;
}

bool Chess::verifyCheckMate() const {
	return verifyCheckMate(0)?true:verifyCheckMate(1);
}

int Chess::winner() const {
	if( verifyCheckMate(0) )
		return 1;
	else if( verifyCheckMate(1) )
		return 0;
	return -1;
}

void Chess::putPieces() {
	Position p(0,0);
	ChessPiece::PieceColor j=ChessPiece::WHITE;
	//arranjar outro jeito
	gameboard->createPiece(Position(1,0),new ChessPiece(ChessPiece::KNIGHT,j));
	gameboard->createPiece(Position(6,0),new ChessPiece(ChessPiece::KNIGHT,j));
	gameboard->createPiece(Position(0,1),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(1,1),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(2,1),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(3,1),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(4,1),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(5,1),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(6,1),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(7,1),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(0,0),new ChessPiece(ChessPiece::ROOK,j));
	gameboard->createPiece(Position(7,0),new ChessPiece(ChessPiece::ROOK,j));
	gameboard->createPiece(Position(2,0),new ChessPiece(ChessPiece::BISHOP,j));
	gameboard->createPiece(Position(5,0),new ChessPiece(ChessPiece::BISHOP,j));
	gameboard->createPiece(Position(3,0),new ChessPiece(ChessPiece::QUEEN,j));
	gameboard->createPiece(Position(4,0),new ChessPiece(ChessPiece::KING,j));
	j=ChessPiece::BLACK;
	gameboard->createPiece(Position(1,7),new ChessPiece(ChessPiece::KNIGHT,j));
	gameboard->createPiece(Position(6,7),new ChessPiece(ChessPiece::KNIGHT,j));
	gameboard->createPiece(Position(0,6),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(1,6),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(2,6),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(3,6),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(4,6),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(5,6),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(6,6),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(7,6),new ChessPiece(ChessPiece::PAWN,j));
	gameboard->createPiece(Position(0,7),new ChessPiece(ChessPiece::ROOK,j));
	gameboard->createPiece(Position(7,7),new ChessPiece(ChessPiece::ROOK,j));
	gameboard->createPiece(Position(2,7),new ChessPiece(ChessPiece::BISHOP,j));
	gameboard->createPiece(Position(5,7),new ChessPiece(ChessPiece::BISHOP,j));
	gameboard->createPiece(Position(3,7),new ChessPiece(ChessPiece::QUEEN,j));
	gameboard->createPiece(Position(4,7),new ChessPiece(ChessPiece::KING,j));
}

//if this function is called, then the move is valid
void Chess::updateState(const ChessMove& j,bool comeu) {
	ChessState* current_state = static_cast<ChessState*>(this->current_state);

	current_state->enpassant=Position(-1,-1);

	if(j.color() == BLACK)
		current_state->fullmoves++;

	current_state->board_fen = this->getPosForFEN();
	current_state->_turn = (this->current_state->_turn == WHITE ? BLACK : WHITE );
	current_state->halfmoves++;

	//FIXME this code is horrible.
	if((this->gameboard->getType(j.to()) == ChessPiece::PAWN) or comeu)
		current_state->halfmoves=0;

	//if the pawn moved 2 squares...
	if(this->gameboard->getType(j.to()) == ChessPiece::PAWN) {
		if( abs(j.to().y - j.from().y) == 2) {
			current_state->enpassant = Position(j.to().x, (int)((j.color() == 0 )? (j.to().y-1) : (j.to().y+1) ) );
		}
	}

	//Has the king moved?
	if(this->gameboard->getType(j.to()) == ChessPiece::KING) {
		char rei, rainha;
		rei = ((j.color() == 0)? 'K' : 'k');
		rainha = ((j.color() == 0) ? 'Q' : 'q');
		if(current_state->castle.find(rei) < current_state->castle.size())
			current_state->castle.erase(current_state->castle.find(rei),1);
		if(current_state->castle.find(rainha) < current_state->castle.size())
			current_state->castle.erase(current_state->castle.find(rainha),1);
		if(current_state->castle.size() == 0)
			current_state->castle+='-';
	}
	//Are the Rooks at their initial places?
	{
		Position p[2];
		char rei = ((j.color() == 0) ? 'K' : 'k');
		char rainha = ((j.color() == 0) ? 'Q' : 'q');
		char torre = ((j.color() == 0) ? 'R' : 'r');
		if(j.color() == 0) {
			p[0]=Position(0,0);p[1]=Position(7,0);
		}
		else {
			p[0]=Position(0,7);p[1]=Position(7,7);
		}
		if ( this->gameboard->getPieceReal(p[0]) != torre ){
			if(current_state->castle.find(rainha) < current_state->castle.size())
				current_state->castle.erase(current_state->castle.find(rainha),1);
		}
		if ( this->gameboard->getPieceReal(p[1]) != torre ){
			if(current_state->castle.find(rei) < current_state->castle.size())
				current_state->castle.erase(current_state->castle.find(rei),1);
		}
		if(current_state->castle.size() == 0)
			current_state->castle="-";
	}
}

void Chess::updateHistory() {
	this->historico->putInHistory(this->current_state);
	this->current_state = new ChessState();
	*(static_cast<ChessState*>(this->current_state)) = *static_cast<const ChessState*>(this->historico->lastState());
}

void Chess::makeMove(const ChessMove &j) const {
	//FIXME workarrounds
	//verify if the move was Castle
	if(this->verifyCastle(j) ) {
		int distx;
		distx=j.to().x - j.from().x ;
		Position aux;
		if(distx < 0)
			aux = Position(0,j.from().y);
		else
			aux = Position(7,j.from().y);
		this->gameboard->makeMove(ChessMove(aux,Position( (j.from().x + j.to().x)/2,j.from().y),j.color()));
	}
	//foi passant?
	if(this->verifyEnPassant(j))
		this->gameboard->createPiece(Position(j.to().x,j.from().y), new ChessPiece());

	this->gameboard->makeMove(j);
}

void Chess::updateMove(const ChessMove &j) {
	bool comeu = verifyEnPassant(j) or (this->gameboard->color(j.to())!=-1);
	this->makeMove(j);

	//Is the Pawn at the end of the tab? 
	//FIXME Default is to transform the pawn to queen
	if(this->gameboard->getType(j.to()) == ChessPiece::PAWN) {
		int final = ( (j.color() == 0) ? 7 : 0);
		if(j.to().y == final) {
			if(j.move().size()==5)
				this->gameboard->createPiece(j.to(),new ChessPiece(ChessPiece::chartotype(j.move()[4]),(ChessPiece::PieceColor)(j.color())));
			else
				this->gameboard->createPiece(j.to(),new ChessPiece(ChessPiece::QUEEN,(ChessPiece::PieceColor)(j.color())));
		}
	}
	this->updateState(j,comeu);
	this->updateHistory();
	this->updateTurn();
}

bool Chess::willBeInCheck(const ChessMove& j) const {
	//TODO a better way to take out mutable from Board
	char a=this->gameboard->getPieceReal(j.from());
	char b=this->gameboard->getPieceReal(j.to());
	bool jogesp=this->verifyCastle(j) or this->verifyEnPassant(j);

	this->makeMove(j);
	bool ans=true;
	if(this->verifyCheck(j.color()) == true)
		ans=false;
	if(jogesp)
		this->setState(current_state->boardFEN());
	else {
		this->gameboard->createPiece(j.from(),new ChessPiece(a));
		this->gameboard->createPiece(j.to(),new ChessPiece(b));
	}
	return ans;
}

bool Chess::verifyMove(const ChessMove &j) const {
	if(this->verifyPieceMove(j))
		return willBeInCheck(j);

	return false;
}

bool Chess::verifyCastle(const ChessMove& j) const {
	if( (this->gameboard->getType(j.from()) == 'K'))
		if( abs(j.to().x - j.from().x)==2 )
			return true;
	return false;
}

bool Chess::verifyCheckMate(int jogador) const {
	if(verifyCheck(jogador) == false)
		return false;

	for(int i=0; i<this->nlines;i++) 
		for(int j=0;j<this->ncolums;j++) 
			if( this->gameboard->color(Position(j,i)) == jogador ) {
				Position onde(j,i);
				std::vector <Position> *p=getPositions(onde);
				if(p->size() > 0) {
					delete p;
					return false;
				}
				delete p;
			}
	return true;
}

bool Chess::verifyDraw(int jogador) const { 
	if(verifyCheck(jogador) == true)
		return false;

	if(static_cast<ChessState*>(current_state)->halfmoves >= 50)
		return true;

	for(int i=0; i<this->nlines;i++) 
		for(int j=0;j<this->ncolums;j++) 
			if( this->gameboard->color(Position(j,i)) == jogador ) {
				Position onde(j,i);
				std::vector <Position> *p=getPositions(onde);
				if(p->size()>0) {
					delete p;
					return false;
				}
				delete p;
			}
	return true;
}
