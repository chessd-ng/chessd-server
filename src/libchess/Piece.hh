#ifndef __PECASH__
#define __PECASH__

#include "ChessMove.hh"

#include <iostream>

class Piece
{
	public:
		//Fazer Assim, dá mais flexibilidade
		enum Piece_type {
			NADA=0,REI=1,RAINHA=2,TORRE=3,BISPO=4,CAVALO=5,PEAO=6
		};
		enum Piece_color {
			BRANCAS=0,PRETAS=1,NENHUM=-1
		};
	private:
		Piece_type tipo;
		Piece_color cor;
	public:
		//funcao por motivos de testes
		void desenha() const;

		char getType() const;
		char getPieceReal() const;
		int getPlayer() const;
		Position getPos() const;
		Piece_type chartotype(char a) const;
		
		//Contrutoras
		Piece(Piece_type t,Piece_color c) {
			this->tipo=t;
			this->cor=c;
		};
		Piece() {
			this->tipo=NADA;
			this->cor=NENHUM;
		}
		Piece(char a);

		~Piece() {
		}
};

#endif
