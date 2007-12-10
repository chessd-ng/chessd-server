#include "Chess.hh"
#include "Piece.hh"
#include "ChessMove.hh"

#include <iostream>

bool Chess::verifyandmakeMove(const std::string& move) {
#ifdef TESTE
	this->Desenha();
#endif
	ChessMove cm((int)vez,move);
	if(verifyMove(cm) == true) {
		updateMove(cm);
		return true;
	}
	return false;
}

void Chess::updateTurn() {
	this->vez=(vez==BRANCAS?PRETAS:BRANCAS);
}

const State& Chess::getState() const {
	return this->atual;
}

bool Chess::verifyCheckMate() const {
	if( verifyCheckMate(0) )
		return true;
	else if( verifyCheckMate(1) )
		return true;
	return false;
}

int Chess::Winner() const {
	if( verifyCheckMate(0) )
		return 1;
	else if( verifyCheckMate(1) )
		return 0;
	return -1;
}

void Chess::putPieces() {
	Position p(0,0);
	Piece::Piece_color j=Piece::BRANCAS;
	//arranjar outro jeito
	gameboard->createPiece(Position(1,0),Piece(Piece::CAVALO,j));
	gameboard->createPiece(Position(6,0),Piece(Piece::CAVALO,j));
	gameboard->createPiece(Position(0,1),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(1,1),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(2,1),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(3,1),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(4,1),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(5,1),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(6,1),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(7,1),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(0,0),Piece(Piece::TORRE,j));
	gameboard->createPiece(Position(7,0),Piece(Piece::TORRE,j));
	gameboard->createPiece(Position(2,0),Piece(Piece::BISPO,j));
	gameboard->createPiece(Position(5,0),Piece(Piece::BISPO,j));
	gameboard->createPiece(Position(3,0),Piece(Piece::RAINHA,j));
	gameboard->createPiece(Position(4,0),Piece(Piece::REI,j));
	j=Piece::PRETAS;
	gameboard->createPiece(Position(1,7),Piece(Piece::CAVALO,j));
	gameboard->createPiece(Position(6,7),Piece(Piece::CAVALO,j));
	gameboard->createPiece(Position(0,6),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(1,6),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(2,6),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(3,6),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(4,6),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(5,6),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(6,6),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(7,6),Piece(Piece::PEAO,j));
	gameboard->createPiece(Position(0,7),Piece(Piece::TORRE,j));
	gameboard->createPiece(Position(7,7),Piece(Piece::TORRE,j));
	gameboard->createPiece(Position(2,7),Piece(Piece::BISPO,j));
	gameboard->createPiece(Position(5,7),Piece(Piece::BISPO,j));
	gameboard->createPiece(Position(3,7),Piece(Piece::RAINHA,j));
	gameboard->createPiece(Position(4,7),Piece(Piece::REI,j));
}
//se veio ate aqui, entao a jogada eh valida
void Chess::updateState(const ChessMove& j,bool comeu) {
	this->atual.lastenpassant = this->atual.enpassant;
	this->atual.enpassant=Position(-1,-1);
	if(j.getPlayer() == Piece::PRETAS)
		this->atual.fullmoves++;
	this->atual.tabfen = this->gameboard->getPosforFEN();
	this->atual.vez = (this->atual.vez == BRANCAS ? PRETAS : BRANCAS );
	this->atual.halfmoves++;
	//FIXME sera que tah bom?
	if((this->gameboard->getType(j.getto()) == 'P') or comeu)
		this->atual.halfmoves=0;
	//se o peao moveu 2 casas...
	if(this->gameboard->getType(j.getto()) == 'P') {
		if( abs(j.getto().posy() - j.getfrom().posy()) == 2) {
			this->atual.enpassant = Position(j.getto().posx(), (int)((j.getPlayer() == 0 )? (j.getto().posy()-1) : (j.getto().posy()+1) ) );
		}
	}

	//Para verificar os casos de castle, necessario verificar se o rei mexeu e torre mexeram
	//Rei Mexeu?
	if(this->gameboard->getType(j.getto()) == 'K') {
		char rei, rainha;
		rei = ((j.getPlayer() == 0)? 'K' : 'k');
		rainha = ((j.getPlayer() == 0) ? 'Q' : 'q');
		if(this->atual.castle.find(rei) < this->atual.castle.size())
			this->atual.castle.erase(this->atual.castle.find(rei),1);
		if(this->atual.castle.find(rainha) < this->atual.castle.size())
			this->atual.castle.erase(this->atual.castle.find(rainha),1);
		if(this->atual.castle.size() == 0)
			this->atual.castle+='-';
	}
	//Are the Rooks at their initial places?
	{
		Position p[2];
		char rei = ((j.getPlayer() == 0) ? 'K' : 'k');
		char rainha = ((j.getPlayer() == 0) ? 'Q' : 'q');
		char torre = ((j.getPlayer() == 0) ? 'R' : 'r');
		if(j.getPlayer() == 0) {
			p[0]=Position(0,0);p[1]=Position(7,0);
		}
		else {
			p[0]=Position(0,7);p[1]=Position(7,7);
		}
		if ( this->gameboard->getPieceReal(p[0]) != torre ){
			if(this->atual.castle.find(rainha) < this->atual.castle.size())
				this->atual.castle.erase(this->atual.castle.find(rainha),1);
		}
		if ( this->gameboard->getPieceReal(p[1]) != torre ){
			if(this->atual.castle.find(rei) < this->atual.castle.size())
				this->atual.castle.erase(this->atual.castle.find(rei),1);
		}
		if(this->atual.castle.size() == 0)
			this->atual.castle="-";
	}
}

void Chess::updateHistory() {
	this->historico.putinHistory(this->atual);
}

void Chess::makeMove(const ChessMove &j) const {
	//FIXME gambiarras
	//Verifica se foi Hook
	if(this->verifyHook(j) ) {
		int distx;
		distx=j.getto().posx() - j.getfrom().posx() ;
		Position aux;
		if(distx < 0)
			aux = Position(0,j.getfrom().posy());
		else
			aux = Position(7,j.getfrom().posy());
		this->gameboard->makeChessMove(ChessMove(aux,Position( (j.getfrom().posx() + j.getto().posx())/2,j.getfrom().posy()),j.getPlayer()));
	}
	//foi passant?
	if(this->verifyEnPassant(j))
		this->gameboard->createPiece(Position(j.getto().posx(),j.getfrom().posy()),Piece(Piece::NADA,Piece::NENHUM));

	this->gameboard->makeChessMove(j);
}

void Chess::updateMove(const ChessMove &j) {
	bool comeu= verifyEnPassant(j) or (this->gameboard->getPlayer(j.getto())!=-1);
	this->makeMove(j);

	//Is the Pawn at the end of the tab? 
	//FIXME gambiarra de transformar peao em rainha
	if(this->gameboard->getType(j.getto()) == 'P') {
		int final = ( (j.getPlayer() == 0) ? 7 : 0);
		if(j.getto().posy() == final) {
			this->gameboard->createPiece(j.getto(),Piece(Piece::RAINHA,(Piece::Piece_color)(j.getPlayer())));
		}
	}
	this->updateState(j,comeu);
	this->updateHistory();
	this->updateTurn();
}
