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

#include "ChessBasedGame.hh"
#include "../Util/utils.hh"

ChessBasedGame::ChessBasedGame(int n, int m) : BoardGame(n,m) {
	this->gameboard=new ChessBoard(n,m);
	this->current_state = new ChessState();
	this->historico = new ChessHistory();
}

ChessBasedGame::~ChessBasedGame() {
	delete gameboard;
	delete current_state;
	delete historico;
}

const std::vector<State*>& ChessBasedGame::getHistory() const {
	return this->historico->getHistory();
}

int ChessBasedGame::numberOfTurns() const {
	return this->historico->getHistory().size()-1;
}

bool ChessBasedGame::verifyCheck(int jogador) const {
	return beingAttacked(findKing(jogador),jogador^1);
}

void ChessBasedGame::setState(const std::string& FEN) const{
	int alinha=7;
	int acoluna=0;
	for(int i=0;i<(int)FEN.size();i++) {
		if((FEN[i] >= '0') and (FEN[i] <= '9')) {
			for( int k = 0; k < FEN[i] - '0';k++) {
				this->gameboard->createPiece(Position(acoluna,alinha), new ChessPiece() );
				acoluna++;
			}
		}
		else if( FEN[i]=='/') {
			acoluna=0;
			alinha--;
		}
		else {
			this->gameboard->createPiece(Position(acoluna,alinha),new ChessPiece(FEN[i]));
			acoluna++;
		}
	}
}

std::string ChessBasedGame::getPosForFEN() const {
	std::string fen;
	int conta;
	for(int i=nlines-1;i>=0;i--) {
		conta=0;
		for(int j=0;j<ncolums;j++) {
			if((*this->gameboard)[i][j]->pieceReal()=='.')
				conta++;
			else {
				if(conta>0)
					fen+=Util::to_string(conta);
				conta=0;
				fen+=(char)((*this->gameboard)[i][j]->pieceReal());
			}
		}
		if(conta>0)
			fen+=Util::to_string(conta);
		
		fen+= '/' ;
	}
	fen.erase(fen.end()-1);
	return fen;
}

bool ChessBasedGame::verifyPieceMove(const ChessMove& j) const {
	Position to = j.to();
	Position from = j.from();
	if((to.x() >=0) and (to.x() < this->ncolums) and (to.y() >= 0) and (to.y() < nlines)) {
		if((j.color()!=(gameboard->color(from)))or(j.color()==(gameboard->color(to))))
			return false;

		//Se arranjar um jeito melhor, eh soh falar =)
		switch (gameboard->getType(from)) {
			case ChessPiece::KNIGHT:
				return verifyHorseMove(j);
			case ChessPiece::PAWN:
				return verifyPawnMove(j);
			case ChessPiece::ROOK:
				return verifyRookMove(j);
			case ChessPiece::BISHOP:
				return verifyBishopMove(j);
			case ChessPiece::QUEEN:
				return verifyQueenMove(j);
			case ChessPiece::KING:
				return verifyKingMove(j);
			default:
				return false;
		};
	}
	return false;
}

bool ChessBasedGame::VerifyDiagonal(const ChessMove& J) const {
	//nao precisa verificar a peca onde esta
	Position para = J.to();
	Position pos = J.from();
	int i,j;
	int k=(para.x()-pos.x());k=k/abs(k);
	int l=(para.y()-pos.y());l=l/abs(l);
	//nao se preocupa com fora das ordas
	for(i=pos.x()+k,j=pos.y()+l;Position(i,j)!=para;i+=k,j+=l)
		if(gameboard->color(Position(i,j)) != -1)
			return false;
	return true;
}

bool ChessBasedGame::VerifyHorizontal(const ChessMove& J) const {
	//FIXME
	//sera q precisa fazer a verificacao se e horizontal aqui ou nao?
	Position para = J.to();
	Position pos = J.from();
	int k=(para.x()-pos.x());k=k/abs(k);
	for(int i=pos.x()+k; i!=para.x() ; i+=k) {
		//acessa tabuleiro e verifica
		if(gameboard->color(Position(i,pos.y())) != -1)
			return false;
	}
	return true;
}

bool ChessBasedGame::VerifyVertical(const ChessMove& J) const {
	Position para = J.to();
	Position pos = J.from();
	int l=(para.y()-pos.y());l=l/abs(l);
	for(int j=pos.y()+l; j!= para.y() ; j+=l) {
		if(gameboard->color(Position(pos.x(),j)) != -1)
			return false;
	}
	return true;
}

bool ChessBasedGame::verifyHorseMove(const ChessMove& j) const {
	int distx,disty;
	distx=abs(j.from().x()-j.to().x());
	disty=abs(j.from().y()-j.to().y());
	if(gameboard->color(j.to()) == j.color())
			return false;
	if( distx+disty == 3 )
		if( (distx==2 and disty==1) or (distx==1 and disty==2) )
			return true;
	return false;
}

bool ChessBasedGame::verifyPawnMove(const ChessMove& jogada) const {
	int distx,disty;
	const Position& from = jogada.from();
	const Position& to = jogada.to();
	distx=abs(from.x()-to.x());
	disty=(to.y()-from.y());

	if( (jogada.color() == 0) && (disty < 0))
		return false;
	else if( (jogada.color() == 1) && (disty > 0))
		return false;
	if(gameboard->color(to) == jogada.color())
		return false;

	disty=abs(disty);

	//Pawn moving just one square
	if( (disty == 1) and (distx == 0) ) {
		if(gameboard->color(to) != -1)
			return false;
		return true;
	}
	//Pawn moving two squares ahead
	else if( (disty == 2) and (distx == 0) ) {
		int meio=(from.y()+to.y())/2;
		if(gameboard->color(Position(from.x(),meio)) != -1)
			return false;
		if(gameboard->color(to) != -1)
			return false;

		//FIXME pouco portavel
		//seria bom gaurdar a posicao inicial y de cada peao
		if(gameboard->color(from) == 0) {
			if(from.y() != 1)
				return false;
		}
		else 
			if(from.y() != this->nlines - 2)
				return false;
		return true;
	}
	else if( (disty == 1) and (distx == 1) ) {
//		if( (gameboard->color(to) == -1) and (gameboard->color(Position(from.x(),to.y())) != (jogada.color()+1)%2 ) )
		if( (this->gameboard->color(to) == -1) ) {
			if( (static_cast<ChessState*>(this->current_state))->enpassant == to)
				return true;
			else
				return false;
		}
		else if(this->gameboard->color(to) != jogada.color())
			return true;			
	}
	else
		return false;

	return true;
}

bool ChessBasedGame::verifyRookMove(const ChessMove& j) const {
	int distx,disty;
	distx=abs(j.from().x()-j.to().x());
	disty=abs(j.from().y()-j.to().y());
//	const Position& from=j.from();
//	const Position& to=j.to();
	if(gameboard->color(j.to()) != j.color())
	{
		if(gameboard->color(j.to()) != j.color())
		{
			if( (distx == 0) and (disty > 0) ) 
				return VerifyVertical(j);
			else if( (distx > 0) and (disty == 0) ) {
				return VerifyHorizontal(j);
			}
		}
	}
	return false;
}

bool ChessBasedGame::verifyBishopMove(const ChessMove& J) const {
	int distx,disty;
	const Position& to = J.to();
	const Position& from = J.from();
	distx=abs(from.x()-to.x());
	disty=abs(from.y()-to.y());
	if(gameboard->color(J.to()) != J.color())
		if( (distx == disty) and (distx!=0) ) 
			return VerifyDiagonal(J);
	
	return false;
}

bool ChessBasedGame::verifyQueenMove(const ChessMove& J) const {
	int distx,disty;
	distx=abs(J.from().x()-J.to().x());
	disty=abs(J.from().y()-J.to().y());

	if(gameboard->color(J.to()) != J.color()) {
		if(distx > 0 or disty > 0) {
			if( distx == disty ) {
				return VerifyDiagonal(J);
			}
			else if( (distx > 0) and (disty == 0)) {
				return VerifyHorizontal(J);
			}
			else if( (disty > 0) and (distx == 0)) {
				return VerifyVertical(J);
			}
		}
	}
	return false;
}

bool ChessBasedGame::verifyKingMove(const ChessMove& J) const {
	if(gameboard->color(J.to()) != J.color()) {
		int distx=(J.from().x()-J.to().x());
		int disty =(J.from().y()-J.to().y());
		if( (abs(distx) <=1) and ( abs(disty) <= 1))
			return true;
		if( ( abs(distx) == 2) and ( abs(disty) == 0) ) {
			char rainha='q',rei='k';
			if(J.color() == WHITE) {
				rainha='Q';	rei='K';
			}
			//foi castle
			std::string castle=(static_cast<ChessState*>(this->current_state))->castle;
			if((distx > 0 and (castle.find(rainha,0) < castle.size())) or ((distx<0)and(castle.find(rei,0) < castle.size()))) {
				//verifica se tem alguem entre a torre e o rei
				if(verifyRookMove(ChessMove(distx>0?Position(0,J.to().y()):Position(7,J.to().y()),Position(J.from().x()-distx/abs(distx),J.to().y()),J.color()))==false)
					return false;
				if(verifyCheck(J.color())) //se o rei tah em cheque
					return false;
				else if(beingAttacked(Position(J.from().x()+J.to().x()/2,J.to().y()),J.color()^1)) //se 1 depois da posicao do rei tah sendo atacada
					return false;
				else if(beingAttacked(J.to(),J.color()^1)) //se 2 depois da posicao do rei tah sendo atacada
					return false;
				return true;
			}
		}
	}
	return false;
}

bool ChessBasedGame::beingAttacked(const Position &onde, int jogador) const {
	for(int i=0;i<this->nlines;i++)
		for(int j=0;j<this->ncolums;j++)
			if((*this->gameboard)[i][j]->color() == jogador)
				if(this->verifyPieceMove(ChessMove(Position(j,i),onde,jogador)))
					return true;
	return false;
}

Position ChessBasedGame::findKing(int player) const {
	for(int i=0; i<this->nlines;i++)
		for(int j=0;j<this->ncolums;j++)
			if( (this->gameboard->getType(Position(j,i)) == ChessPiece::KING) and (this->gameboard->color(Position(j,i)) == player) )
				return Position(j,i);
	return Position(-1,-1);
}

//this function does not verify anything
bool ChessBasedGame::verifyEnPassant(const ChessMove& j) const {
	if(this->gameboard->getType(j.from()) == ChessPiece::PAWN) {
		int distx=abs(j.to().x() - j.from().x() );
		int disty=abs(j.to().y() - j.from().y() );
		if( (distx == 1) and (disty == 1) and (this->gameboard->color(j.to()) == -1) ) {
			return true;
		}
	}
	return false;
}

std::vector <Position> *ChessBasedGame::getVerticalandHorizontal(const Position& p) const {
	std::vector <Position> *ans=new std::vector <Position>;
	int py[]={1,-1,0,0};
	int px[]={0,0,1,-1};
	for(int k=0;k<4;k++) {
		for(int i=(p.y()+py[k]),j=(p.x()+px[k]);(i<this->nlines) and (i>=0) and (j < this->ncolums) and (j>=0);i+=py[k],j+=px[k]) {
			Position aux(j,i);
			if(this->gameboard->color(aux) == -1)
				ans->push_back(aux);
			else break;
		}
	}
	return ans;
}

std::vector <Position> *ChessBasedGame::getDiagonals(const Position& p) const {
	std::vector <Position> *ans = new std::vector <Position>;
	int dx[]={1,-1,1,-1};
	int dy[]={1,1,-1,-1};
	for(int k=0;k<4;k++) {
		for(int i=(p.y()+dy[k]),j=(p.x()+dx[k]);(i<this->ncolums) and(i>=0) and (j < this->ncolums) and (j>=0);i+=dy[k],j+=dx[k]) {
			Position aux(j,i);
			if( this->gameboard->color(aux) == -1)
				ans->push_back(aux);
			else break;
		}
	}
	return ans;
}

std::vector <Position> *ChessBasedGame::getPositions(const Position& onde) const {
	int jogador = this->gameboard->color(onde);
	std::vector <Position> *pos = this->getpossiblePositions(onde);
	std::vector <Position> *ans = new std::vector <Position>;
	for(int i=0;i<(int)pos->size();i++) {
		if(verifyMove( ChessMove(onde,(*pos)[i],jogador) ))
			ans->push_back( (*pos)[i] );
	}
	delete pos;
	return ans;
}

std::vector <Position> *ChessBasedGame::getpossiblePositions(const Position& onde) const {

	std::vector <Position> *p = new std::vector <Position>;
	switch(this->gameboard->getType(onde)) {
		case ChessPiece::KNIGHT:
			{
				int posx[]={1,2,2,1,-1,-2,-2,-1};
				int posy[]={2,1,-1,-2,-2,-1,1,2};
				for(int k=0;k<8;k++)
					p->push_back(Position(posx[k],posy[k])+onde);
			}
			break;
		case ChessPiece::PAWN:
			{
				int posx[]={0,0,1,-1,0,0,1,-1};
				int posy[]={1,2,1,1,-1,-2,-1,-1};
				for(int k=0;k<8;k++)
					p->push_back(Position(posx[k],posy[k])+onde);
			}
			break;
		case ChessPiece::ROOK:
			{
				std::vector <Position> *pos = getVerticalandHorizontal(onde);
				p->insert(p->end(),pos->begin(),pos->end());
				delete pos;
			}
			break;
		case ChessPiece::BISHOP:
			{
				std::vector <Position> *pos = getDiagonals(onde);
				p->insert(p->end(),pos->begin(),pos->end());
				delete pos;
			}
			break;
		case ChessPiece::QUEEN:
			{
				std::vector <Position> *pos = getVerticalandHorizontal(onde);
				p->insert(p->end(),pos->begin(),pos->end());
				delete pos;
				pos = getDiagonals(onde);
				p->insert(p->end(),pos->begin(),pos->end());
				delete pos;
			}
			break;
		case ChessPiece::KING:
			{
				int posx[]={0,1,1,1,0,-1,-1,-1,2,-2};
				int posy[]={1,1,0,-1,-1,-1,0,1,0,0};
				for(int k=0;k<10;k++)
					p->push_back(Position(posx[k],posy[k])+onde);
			}
			break;
		default:
			break;
	}
	return p;
}
