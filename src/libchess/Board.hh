#ifndef _TABULEIROH_
#define _TABULEIROH_

#include "Piece.hh"

#include <iostream>

class Board
{
	private:
		int nlines,ncolums;
		Piece** board;
	public:
		Board(int n,int m); 
		~Board();
#ifdef DEBUG
		//para testes
		void desenha() const;
#endif

		//pega o FEN completo
		std::string getFEN() const;

		//Pega apenas a parte das posicoes das pegas do FEN
		std::string getPosforFEN() const;
		void createPiece(const Position& pos,const Piece& pe);

		//Funcoes para pegar informacoes do boardleiro
		char getType(const Position& p) const;
		char getPieceReal(const Position& p) const;
		int getPlayer(const Position& p) const;

		//essa funcao estabelece o boardleiro a partir do FEN dado
		void setState(const std::string& FEN);
		//Executa uma ChessMove
		void makeChessMove(const ChessMove& j);
};

#endif
