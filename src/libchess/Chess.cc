#include "Chess.hh"
#include "Piece.hh"
#include "ChessMove.hh"
#include <vector>
#include <stdlib.h>


bool Chess::verifyandmakeMove(const std::string& move) {
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

const State& Chess::getState() const {
	return this->atual;
}
const std::vector<State> &Chess::getHistory() const {
	return this->historico.getHistory();
}

bool Chess::verifyDraw() const {
	return verifyDraw(0)==true?true:verifyDraw(1);
}
int Chess::Turn() const {
	return _turn;
}

bool Chess::verifyCheckMate() const {
	return verifyCheckMate(0)?true:verifyCheckMate(1);
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
//se veio ate aqui, entao a jogada eh valida
void Chess::updateState(const ChessMove& j,bool comeu) {
	this->atual.lastenpassant = this->atual.enpassant;
	this->atual.enpassant=Position(-1,-1);
	if(j.getPlayer() == BLACK)
		this->atual.fullmoves++;
	this->atual.tabfen = this->getPosForFEN();
	this->atual.vez = (this->atual.vez == WHITE ? BLACK : WHITE );
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
		this->gameboard->createPiece(Position(j.getto().posx(),j.getfrom().posy()), new ChessPiece());

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
			this->gameboard->createPiece(j.getto(),ChessPiece(ChessPiece::QUEEN,(ChessPiece::PieceColor)(j.getPlayer())));
		}
	}
	this->updateState(j,comeu);
	this->updateHistory();
	this->updateTurn();
}

bool Chess::verifyMove(const ChessMove &j) const {
	if(this->verifyPieceMove(j))
	{
		//TODO um jeito melhor
		char a=this->gameboard->getPieceReal(j.getfrom());
		char b=this->gameboard->getPieceReal(j.getto());
		bool jogesp=this->verifyHook(j) or this->verifyEnPassant(j);

		this->makeMove(j);
		bool ans=true;
		if(this->verifyCheck(j.getPlayer()) == true)
			ans=false;
		if(jogesp)
			this->gameboard->setState(this->atual.getFEN());
		else {
			this->gameboard->createPiece(j.getfrom(),Piece(a));
			this->gameboard->createPiece(j.getto(),Piece(b));
		}
		return ans;
	}
	return false;
}

bool Chess::verifyHook(const ChessMove& j) const {
	if( (this->gameboard->getType(j.getfrom()) == 'K'))
		if( abs(j.getto().posx() - j.getfrom().posx())==2 )
			return true;
	return false;
}

bool Chess::verifyCheckMate(int jogador) const {
	if(verifyCheck(jogador) == false)
		return false;

	for(int i=0; i<this->nlines;i++) 
		for(int j=0;j<this->ncolums;j++) 
			if( this->gameboard->getPlayer(Position(j,i)) == jogador ) {
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
	if(atual.halfmoves >= 50)
		return true;
	if(verifyCheck(jogador) == true)
		return false;
	for(int i=0; i<this->nlines;i++) 
		for(int j=0;j<this->ncolums;j++) 
			if( this->gameboard->getPlayer(Position(j,i)) == jogador ) {
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
bool Chess::verifyCheck(int jogador) const {
	return beingAttacked(findKing(jogador),jogador^1);
}
