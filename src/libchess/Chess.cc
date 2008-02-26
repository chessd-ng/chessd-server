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
	this->history->putInHistory(this->current_state);
	this->current_state=new ChessState();
	this->_turn=WHITE;
}

Chess::Chess(const std::string& FEN) : ChessBasedGame(8,8) {
	this->current_state=new ChessState(FEN);

	this->_turn=current_state->turn();

	this->history->putInHistory(new ChessState(FEN));

	this->setState(FEN.substr(0,FEN.find(' ')));
}

int Chess::winner() const {
	if( verifyCheckMate(0) )
		return BLACK;
	else if( verifyCheckMate(1) )
		return WHITE;
	return UNDEFINED;
}

int Chess::turn() const {
	return _turn;
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
	if(move[3]<'1' or move[3]>'8')
		return false;
	if(move.size()==5)
		if(toupper(move[4])!='Q' and toupper(move[4])!='N' and toupper(move[4])!='B' and toupper(move[4])!='R')
			return false;

	ChessMove cm(_turn,move);
	if(verifyMove(cm) == true) {
		updateMove(cm);
		return true;
	}
	return false;
}

bool Chess::verifyCheckMate() const {
	return verifyCheckMate(WHITE)?true:verifyCheckMate(BLACK);
}

int Chess::verifyDraw() const {
	if(verifyThreefoldRepetition()==true)
		return 1;

	if(verifyImpossibilityOfCheckmate()==true)
		return 2;

	if(static_cast<ChessState*>(current_state)->halfmoves >= 50)
		return 3;

	if(verifyStaleMate(this->_turn)==true)
		return 4;

	return 0;
}

const ChessState& Chess::getState() const {
	return *(static_cast<ChessState*>(this->current_state));
}

void Chess::updateHistory() {
	this->history->putInHistory(this->current_state);
	this->current_state = new ChessState();
	*(static_cast<ChessState*>(this->current_state)) = *static_cast<const ChessState*>(this->history->lastState());
}

void Chess::updateTurn() {
	this->_turn=(this->_turn==WHITE?BLACK:WHITE);
}

bool Chess::verifyCheckMate(int player) const {
	if(verifyCheck(player) == false)
		return false;

	for(int i=0; i<this->nlines;i++) 
		for(int j=0;j<this->ncolums;j++) 
			if( this->gameboard->color(Position(j,i)) == player ) {
				Position pos(j,i);
				std::vector <Position> *p=getPositions(pos);
				if(p->size() > 0) {
					delete p;
					return false;
				}
				delete p;
			}
	return true;
}

bool Chess::verifyMove(const ChessMove &mv) const {
	if(this->verifyPieceMove(mv))
		return willBeInCheck(mv);

	return false;
}

bool Chess::willBeInCheck(const ChessMove& mv) const {
	//TODO a better way to take out mutable from Board
	char a=this->gameboard->getPieceReal(mv.from());
	char b=this->gameboard->getPieceReal(mv.to());
	bool specialmv=this->verifyCastle(mv) or this->verifyEnPassant(mv);

	this->makeMove(mv);
	bool ans=true;
	if(this->verifyCheck(mv.color()) == true)
		ans=false;
	if(specialmv)
		this->setState(current_state->boardFEN());
	else {
		this->gameboard->createPiece(mv.from(),new ChessPiece(a));
		this->gameboard->createPiece(mv.to(),new ChessPiece(b));
	}
	return ans;
}

void Chess::makeMove(const ChessMove &mv) const {
	//FIXME workarrounds
	//verify if the move was castle
	if(this->verifyCastle(mv) ) {
		int distx;
		distx=mv.to().x - mv.from().x ;
		Position aux;
		if(distx < 0)
			aux = Position(0,mv.from().y);
		else
			aux = Position(7,mv.from().y);
		this->gameboard->makeMove(ChessMove(aux,Position( (mv.from().x + mv.to().x)/2,mv.from().y),mv.color()));
	}
	//enpassant
	if(this->verifyEnPassant(mv))
		this->gameboard->createPiece(Position(mv.to().x,mv.from().y), new ChessPiece());

	this->gameboard->makeMove(mv);
}

void Chess::updateMove(const ChessMove &mv) {
	bool has_eaten = verifyEnPassant(mv) or (this->gameboard->color(mv.to())!=-1);
	this->makeMove(mv);

	//Is the Pawn at the end of the tab? 
	//Default is to transform the pawn to queen
	//or get the piece at the end of move
	if(this->gameboard->getType(mv.to()) == ChessPiece::PAWN) {
		int final = ( (mv.color() == 0) ? 7 : 0);
		if(mv.to().y == final) {
			if(mv.move().size()==5)
				this->gameboard->createPiece(mv.to(),new ChessPiece(ChessPiece::chartotype(mv.move()[4]),(ChessPiece::PieceColor)(mv.color())));
			else
				this->gameboard->createPiece(mv.to(),new ChessPiece(ChessPiece::QUEEN,(ChessPiece::PieceColor)(mv.color())));
		}
	}
	this->updateState(mv,has_eaten);
	this->updateHistory();
	this->updateTurn();
}

//if this function is called, then the move is valid
void Chess::updateState(const ChessMove& j, bool has_eaten) {
	ChessState* current_state = static_cast<ChessState*>(this->current_state);

	current_state->enpassant=Position(-1,-1);

	if(j.color() == BLACK)
		current_state->fullmoves++;

	current_state->board_fen = this->getPosForFEN();
	current_state->_turn = (this->current_state->_turn == WHITE ? BLACK : WHITE );
	current_state->halfmoves++;

	//FIXME this code is horrible.
	if((this->gameboard->getType(j.to()) == ChessPiece::PAWN) or has_eaten)
		current_state->halfmoves=0;

	//if the pawn moved 2 squares...
	if(this->gameboard->getType(j.to()) == ChessPiece::PAWN) {
		if( abs(j.to().y - j.from().y) == 2) {
			current_state->enpassant = Position(j.to().x, (int)((j.color() == 0 )? (j.to().y-1) : (j.to().y+1) ) );
		}
	}

	//Has the king moved?
	if(this->gameboard->getType(j.to()) == ChessPiece::KING) {
		char king, queen;
		king = ((j.color() == 0)? 'K' : 'k');
		queen = ((j.color() == 0) ? 'Q' : 'q');
		if(current_state->castle.find(king) < current_state->castle.size())
			current_state->castle.erase(current_state->castle.find(king),1);
		if(current_state->castle.find(queen) < current_state->castle.size())
			current_state->castle.erase(current_state->castle.find(queen),1);
		if(current_state->castle.size() == 0)
			current_state->castle+='-';
	}
	//Are the Rooks at their initial places?
	{
		Position p[2];
		char king = ((j.color() == 0) ? 'K' : 'k');
		char queen = ((j.color() == 0) ? 'Q' : 'q');
		if(j.color() == 0) {
			p[0]=Position(0,0);p[1]=Position(7,0);
		}
		else {
			p[0]=Position(0,7);p[1]=Position(7,7);
		}
		if ( this->gameboard->getType(p[0]) != ChessPiece::ROOK ){
			if(current_state->castle.find(queen) < current_state->castle.size())
				current_state->castle.erase(current_state->castle.find(queen),1);
		}
		if ( this->gameboard->getType(p[1]) != ChessPiece::ROOK ){
			if(current_state->castle.find(king) < current_state->castle.size())
				current_state->castle.erase(current_state->castle.find(king),1);
		}
		if(current_state->castle.size() == 0)
			current_state->castle="-";
	}
}

bool Chess::verifyCastle(const ChessMove& mv) const {
	if( (this->gameboard->getType(mv.from()) == ChessPiece::KING))
		if( abs(mv.to().x - mv.from().x)==2 )
			return true;
	return false;
}

bool Chess::verifyStaleMate(int player) const { 
	if(verifyCheck(player) == true)
		return false;

	for(int i=0; i<this->nlines;i++) 
		for(int j=0;j<this->ncolums;j++) 
			if( this->gameboard->color(Position(j,i)) == player ) {
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

bool Chess::verifyThreefoldRepetition() const {
	int count=1;
	ChessHistory *history=static_cast<ChessHistory*>(this->history);
	//only needs to verify "halfmoves" for equal states
	for(int i=1;i<=(*history)[history->size()-1].halfmoves;i++)
		if((*history)[history->size()-1-i]==(*history)[history->size()-1])
			count++;
	if(count>=3)
		return true;
	return false;
}

bool Chess::verifyImpossibilityOfCheckmate() const {
	std::vector<std::pair<ChessPiece,Position> > aux[2];
	for(int i=0;i<this->nlines;i++)
		for(int j=0;j<this->ncolums;j++) {
			if((*this->gameboard)[i][j]->type() != ChessPiece::NOTYPE) {
				aux[(*this->gameboard)[i][j]->color()].push_back(std::make_pair(*static_cast<ChessPiece*>((*this->gameboard)[i][j]),Position(j,i)));
				if(aux[(*this->gameboard)[i][j]->color()].size() > 2)
					return false;
			}
		}

	if((aux[0].size()==1) and (aux[1].size()==1))
		return true;
	else if((aux[0].size()==1) and (aux[1].size()==2)) {
		for(int i=0;i<(int)aux[1].size();i++)
			if(aux[1][i].first.type()==ChessPiece::BISHOP or aux[1][i].first.type()==ChessPiece::KNIGHT)
				return true;
	}

	else if((aux[0].size()==2) and (aux[1].size()==1)) {
		for(int i=0;i<(int)aux[0].size();i++)
			if(aux[0][i].first.type()==ChessPiece::BISHOP or aux[0][i].first.type()==ChessPiece::KNIGHT)
				return true;
	}

	else if((aux[0].size()==2) and (aux[1].size()==2)) {
		for(int i=0;i<(int)aux[0].size();i++)
			if(aux[0][i].first.type()==ChessPiece::BISHOP)
				for(int j=0;j<(int)aux[1].size();j++)
					if(aux[1][j].first.type()==ChessPiece::BISHOP)
						if((aux[0][i].second.x+aux[0][i].second.y)%2 == (aux[1][j].second.x+aux[1][j].second.y)%2)
							return true;
	}
	return false;
}

void Chess::putPieces() {
	Position p(0,0);
	ChessPiece::PieceColor j=ChessPiece::WHITE;
	//maybe just set state is fine
	this->gameboard->createPiece(Position(1,0),new ChessPiece(ChessPiece::KNIGHT,j));
	this->gameboard->createPiece(Position(6,0),new ChessPiece(ChessPiece::KNIGHT,j));
	this->gameboard->createPiece(Position(0,1),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(1,1),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(2,1),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(3,1),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(4,1),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(5,1),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(6,1),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(7,1),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(0,0),new ChessPiece(ChessPiece::ROOK,j));
	this->gameboard->createPiece(Position(7,0),new ChessPiece(ChessPiece::ROOK,j));
	this->gameboard->createPiece(Position(2,0),new ChessPiece(ChessPiece::BISHOP,j));
	this->gameboard->createPiece(Position(5,0),new ChessPiece(ChessPiece::BISHOP,j));
	this->gameboard->createPiece(Position(3,0),new ChessPiece(ChessPiece::QUEEN,j));
	this->gameboard->createPiece(Position(4,0),new ChessPiece(ChessPiece::KING,j));
	j=ChessPiece::BLACK;
	this->gameboard->createPiece(Position(1,7),new ChessPiece(ChessPiece::KNIGHT,j));
	this->gameboard->createPiece(Position(6,7),new ChessPiece(ChessPiece::KNIGHT,j));
	this->gameboard->createPiece(Position(0,6),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(1,6),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(2,6),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(3,6),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(4,6),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(5,6),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(6,6),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(7,6),new ChessPiece(ChessPiece::PAWN,j));
	this->gameboard->createPiece(Position(0,7),new ChessPiece(ChessPiece::ROOK,j));
	this->gameboard->createPiece(Position(7,7),new ChessPiece(ChessPiece::ROOK,j));
	this->gameboard->createPiece(Position(2,7),new ChessPiece(ChessPiece::BISHOP,j));
	this->gameboard->createPiece(Position(5,7),new ChessPiece(ChessPiece::BISHOP,j));
	this->gameboard->createPiece(Position(3,7),new ChessPiece(ChessPiece::QUEEN,j));
	this->gameboard->createPiece(Position(4,7),new ChessPiece(ChessPiece::KING,j));
}
