#include "Board.hh"
#include <string>

//apenas para testes
#include <iostream>

std::string toString(int n) {
	std::string ans;
	while(n>0) {
		ans+= (char)(n%10 + '0');
		n/=10;
	}
	return ans;
}

void Board::setState(const std::string& FEN) {
	int alinha=7;
	int acoluna=0;
	for(int i=0;i<(int)FEN.size();i++) {
		if((FEN[i] >= '0') and (FEN[i] <= '9')) {
			for( int k = 0; k < FEN[i] - '0';k++) {
				this->createPiece(Position(acoluna,alinha),Piece(Piece::NADA,Piece::NENHUM));
				acoluna++;
			}
		}
		else if( FEN[i]=='/') {
			acoluna=0;
			alinha--;
		}
		else {
			this->createPiece(Position(acoluna,alinha),FEN[i]);
			acoluna++;
		}
	}
}

#ifdef TESTE
void Board::desenha() const {
	for(int i=nlines-1;i>=0;i--) {
		std::cout << i+1 << " ";
		for(int j=0;j<ncolums;j++)
			this->board[i][j].desenha();
		std::cout << std::endl;
	}
	std::cout << "  ";
	for(int j=0;j<ncolums;j++)
		std::cout << (char)('a'+j) << " ";
	std::cout << std::endl;
#ifdef DEBUG
	std::cout << getPosforFEN() << std::endl;
#endif
	std::cout << "-----------------\n";
}
#endif

std::string Board::getPosforFEN() const {
	std::string fen;
	int conta;
	for(int i=nlines-1;i>=0;i--) {
		conta=0;
		for(int j=0;j<ncolums;j++) {
			if(board[i][j].getPieceReal()=='.')
				conta++;
			else {
				if(conta>0)
					fen+=toString(conta);
				conta=0;
				fen+=board[i][j].getPieceReal();
			}
		}
		if(conta>0)
			fen+=toString(conta);
		
		fen+= '/' ;
	}
	fen.erase(fen.end()-1);
	return fen;
}

Board::Board(int n,int m) {
	this->nlines=n;
	this->ncolums=m;
	board = new Piece*[nlines];
	for(int i=0;i<nlines;i++)
		board[i] = new Piece[ncolums];
}
Board::~Board() {
	for(int i=0;i<nlines;i++)
		delete [] board[i];
	delete [] board;
}

void Board::createPiece(const Position& pos,const Piece& pe) {
	this->board[pos.posy()][pos.posx()]=pe;
}

void Board::makeChessMove(const ChessMove& j)
{
	//FIXME
	int tox=j.getto().posx(),toy=j.getto().posy();
	int fromx=j.getfrom().posx(),fromy=j.getfrom().posy();
	this->board[toy][tox] = board[fromy][fromx];
	this->board[fromy][fromx]=Piece(Piece::NADA,Piece::NENHUM);
}

char Board::getType(const Position& p) const {
	return board[p.posy()][p.posx()].getType();
}
char Board::getPieceReal(const Position& p) const {
	return board[p.posy()][p.posx()].getPieceReal();
}
int Board::getPlayer(const Position& p) const {
	return board[p.posy()][p.posx()].getPlayer();
}

