#include "Chess.hh"
#include <vector>
#include <stdlib.h>
bool Chess::verifyPieceMove(const ChessMove& j) const {
	Position to = j.getto();
	Position from = j.getfrom();
	if((to.posx() >=0) and (to.posx() < this->ncolums) and (to.posy() >= 0) and (to.posy() < nlines)) {
		if((j.getPlayer()!=(gameboard->getPlayer(from)))or(j.getPlayer()==(gameboard->getPlayer(to))))
			return false;

		//Se arranjar um jeito melhor, eh soh falar =)
		switch (gameboard->getType(from)) {
			case 'N':
				return verifyHorseMove(j);
			case 'P':
				return verifyPawnMove(j);
			case 'R':
				return verifyRookMove(j);
			case 'B':
				return verifyBishopMove(j);
			case 'Q':
				return verifyQueenMove(j);
			case 'K':
				return verifyKingMove(j);
			default:
				return false;
		};
	}
	return false;
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
//this function does not verify anything
bool Chess::verifyEnPassant(const ChessMove& j) const {
	if(this->gameboard->getType(j.getfrom()) == 'P') {
		int distx=abs(j.getto().posx() - j.getfrom().posx() );
		int disty=abs(j.getto().posy() - j.getfrom().posy() );
		if( (distx == 1) and (disty == 1) and (this->gameboard->getPlayer(j.getto()) == -1) ) {
			return true;
		}
	}
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
Position Chess::findKing(int player) const {
	for(int i=0; i<this->nlines;i++)
		for(int j=0;j<this->ncolums;j++)
			if( (this->gameboard->getType(Position(j,i)) == 'K') and (this->gameboard->getPlayer(Position(j,i)) == player) )
				return Position(j,i);
	return Position(-1,-1);
}
bool Chess::verifyCheck(int jogador) const {
	//ver se da para colocar o numero de linhas e colunas
	//guardar posiçao do rei eh importante
	return beingAttacked(findKing(jogador),jogador^1);
	/*
	//verifica vertical e horizontal Para ver se torre ou rainha estao atacando
	int py[]={1,-1,0,0};
	int px[]={0,0,1,-1};
	for(int k=0;k<4;k++) {
		for(int i=(onde.posy()+py[k]),j=(onde.posx()+px[k]);(i<this->nlines) and (i>=0) and (j < this->ncolums) and (j>=0);i+=py[k],j+=px[k]) {
			Position aux(j,i);
			if( (this->gameboard->getPlayer(aux) != jogador) and ((this->gameboard->getType(aux) == 'R') or (this->gameboard->getType(aux) == 'Q')) )
				return true;
			else if(this->gameboard->getPlayer(aux) != -1)
				break;
		}
	}
	//verifica diagonais Para ver se rainha ou bispo estao atacando
	int dx[]={1,-1,1,-1};
	int dy[]={1,1,-1,-1};
	for(int k=0;k<4;k++) {
		for(int i=(onde.posy()+dy[k]),j=(onde.posx()+dx[k]);(i<this->ncolums) and(i>=0) and (j < this->ncolums) and (j>=0);i+=dy[k],j+=dx[k]) {
			Position aux(j,i);
			if( (this->gameboard->getPlayer(aux) != jogador) and ((this->gameboard->getType(aux) == 'B') or (this->gameboard->getType(aux) == 'Q')))
				return true;
			else if(this->gameboard->getPlayer(aux) != -1)
				break;
		}
	}
	//verifica cavalo
	{
		//sentido horario contando a partir de cima
		int posx[]={1,2,2,1,-1,-2,-2,-1};
		int posy[]={2,1,-1,-2,-2,-1,1,2};
		for(int i=0;i<8;i++) {
			Position aux(onde.posx() + posx[i], onde.posy()+posy[i]);
			if( (aux.posx() >= 0) and (aux.posx() < this->ncolums) and (aux.posy() >= 0) and (aux.posy() < this->nlines) ) {
				if((this->gameboard->getType(aux) == 'N') and (this->gameboard->getPlayer(aux) != jogador))
					return true;
			}
		}
	}
	//verifica rei
	{
		//sentido horario contando a partir de cima
		int posx[]={0,1,1,1,0,-1,-1,-1};
		int posy[]={1,1,0,-1,-1,-1,0,1};
		for(int i=0;i<8;i++) {
			Position aux(onde.posx() + posx[i], onde.posy()+posy[i]);
			if( (aux.posx() >= 0) and (aux.posx() < this->ncolums) and (aux.posy() >= 0) and (aux.posy() < this->nlines) ) {
				if((this->gameboard->getType(aux) == 'K') and (this->gameboard->getPlayer(aux) != jogador))
					return true;
			}
		}

	}
	//verifica peao
	{
		int tmp;
		tmp = (jogador == 0 )?( 1):(-1);
		if((onde.posy()+tmp >= 0) and (onde.posy()+tmp < this->nlines)) {
			Position posagora(onde.posx()-1,onde.posy()+tmp);
			if( (this->gameboard->getType(posagora) == 'P') and (this->gameboard->getPlayer(posagora) != jogador))
				return true;
			posagora=Position(onde.posx()+1,onde.posy()+tmp);
			if( (this->gameboard->getType(posagora) == 'P') and (this->gameboard->getPlayer(posagora) != jogador))
				return true;
		}
	}
	*/

	return false;
}

bool Chess::verifyHorseMove(const ChessMove& j) const {
	int distx,disty;
	distx=abs(j.getfrom().posx()-j.getto().posx());
	disty=abs(j.getfrom().posy()-j.getto().posy());
	if(gameboard->getPlayer(j.getto()) == j.getPlayer())
			return false;
	if( distx+disty == 3 )
		if( (distx==2 and disty==1) or (distx==1 and disty==2) )
			return true;
	return false;
}

bool Chess::verifyPawnMove(const ChessMove& jogada) const {
	int distx,disty;
	const Position& from = jogada.getfrom();
	const Position& to = jogada.getto();
	distx=abs(from.posx()-to.posx());
	disty=(to.posy()-from.posy());

	if( (jogada.getPlayer() == 0) && (disty < 0))
		return false;
	else if( (jogada.getPlayer() == 1) && (disty > 0))
		return false;
	if(gameboard->getPlayer(to) == jogada.getPlayer())
		return false;

	disty=abs(disty);

	//ChessMove normal de uma posicao a frente
	if( (disty == 1) and (distx == 0) ) {
		if(gameboard->getPlayer(to) != -1)
			return false;
		return true;
	}
	//ChessMove normal de duas posicoes a frente
	else if( (disty == 2) and (distx == 0) ) {
		int meio=(from.posy()+to.posy())/2;
		if(gameboard->getPlayer(Position(from.posx(),meio)) != -1)
			return false;
		if(gameboard->getPlayer(to) != -1)
			return false;

		//FIXME pouco portavel
		//seria bom gaurdar a posicao inicial y de cada peao
		if(gameboard->getPlayer(from) == 0) {
			if(from.posy() != 1)
				return false;
		}
		else 
			if(from.posy() != this->nlines - 2)
				return false;
		return true;
	}
	else if( (disty == 1) and (distx == 1) ) {
//		if( (gameboard->getPlayer(to) == -1) and (gameboard->getPlayer(Position(from.posx(),to.posy())) != (jogada.getPlayer()+1)%2 ) )
		if( (this->gameboard->getPlayer(to) == -1) ) {
			if( this->atual.enpassant == to)
				return true;
			else
				return false;
		}
		else if(this->gameboard->getPlayer(to) != jogada.getPlayer())
			return true;			
	}
	else
		return false;

	return true;
}

bool Chess::verifyRookMove(const ChessMove& j) const {
	int distx,disty;
	distx=abs(j.getfrom().posx()-j.getto().posx());
	disty=abs(j.getfrom().posy()-j.getto().posy());
//	const Position& from=j.getfrom();
//	const Position& to=j.getto();
	if(gameboard->getPlayer(j.getto()) != j.getPlayer())
	{
		if(gameboard->getPlayer(j.getto()) != j.getPlayer())
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

bool Chess::verifyBishopMove(const ChessMove& J) const {
	int distx,disty;
	const Position& to = J.getto();
	const Position& from = J.getfrom();
	distx=abs(from.posx()-to.posx());
	disty=abs(from.posy()-to.posy());
	if(gameboard->getPlayer(J.getto()) != J.getPlayer())
		if( (distx == disty) and (distx!=0) ) 
			return VerifyDiagonal(J);
	
	return false;
}

bool Chess::verifyQueenMove(const ChessMove& J) const {
	int distx,disty;
	distx=abs(J.getfrom().posx()-J.getto().posx());
	disty=abs(J.getfrom().posy()-J.getto().posy());

	if(gameboard->getPlayer(J.getto()) != J.getPlayer()) {
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

bool Chess::beingAttacked(const Position &onde, int jogador) const {
	for(int i=0;i<this->nlines;i++)
		for(int j=0;j<this->ncolums;j++)
			if(this->gameboard->getPlayer(Position(j,i)) == jogador)
				if(this->verifyPieceMove(ChessMove(Position(j,i),onde,jogador)))
					return true;
	return false;
}
bool Chess::verifyKingMove(const ChessMove& J) const {
	if(gameboard->getPlayer(J.getto()) != J.getPlayer()) {
		int distx=(J.getfrom().posx()-J.getto().posx());
		int disty =(J.getfrom().posy()-J.getto().posy());
		if( (abs(distx) <=1) and ( abs(disty) <= 1))
			return true;
		if( ( abs(distx) == 2) and ( abs(disty) == 0) ) {
			char rainha='q',rei='k';
			if(J.getPlayer() == 0) {
				rainha='Q';	rei='K';
			}
			//foi castle
			if((distx > 0 and (atual.castle.find(rainha,0) < atual.castle.size())) or ((distx<0)and(atual.castle.find(rei,0) < atual.castle.size()))) {
				//verifica se tem alguem entre a torre e o rei
				if(verifyRookMove(ChessMove(distx>0?Position(0,J.getto().posy()):Position(7,J.getto().posy()),Position(J.getfrom().posx()-distx/abs(distx),J.getto().posy()),J.getPlayer()))==false)
					return false;
				if(verifyCheck(J.getPlayer())) //se o rei tah em cheque
					return false;
				else if(beingAttacked(Position(J.getfrom().posx()+J.getto().posx()/2,J.getto().posy()),J.getPlayer()^1)) //se 1 depois da posicao do rei tah sendo atacada
					return false;
				else if(beingAttacked(J.getto(),J.getPlayer()^1)) //se 2 depois da posicao do rei tah sendo atacada
					return false;
				return true;
			}
		}
	}
	return false;
}

bool BoardGame::VerifyDiagonal(const ChessMove& J) const {
	//nao precisa verificar a peca onde esta
	Position para = J.getto();
	Position pos = J.getfrom();
	int i,j;
	int k=(para.posx()-pos.posx());k=k/abs(k);
	int l=(para.posy()-pos.posy());l=l/abs(l);
	//nao se preocupa com fora das ordas
	for(i=pos.posx()+k,j=pos.posy()+l;Position(i,j)!=para;i+=k,j+=l)
		if(gameboard->getPlayer(Position(i,j)) != -1)
			return false;
	return true;
}

bool BoardGame::VerifyHorizontal(const ChessMove& J) const {
	//FIXME
	//sera q precisa fazer a verificacao se e horizontal aqui ou nao?
	Position para = J.getto();
	Position pos = J.getfrom();
	int k=(para.posx()-pos.posx());k=k/abs(k);
	for(int i=pos.posx()+k; i!=para.posx() ; i+=k) {
		//acessa tabuleiro e verifica
		if(gameboard->getPlayer(Position(i,pos.posy())) != -1)
			return false;
	}
	return true;
}

bool BoardGame::VerifyVertical(const ChessMove& J) const {
	Position para = J.getto();
	Position pos = J.getfrom();
	int l=(para.posy()-pos.posy());l=l/abs(l);
	for(int j=pos.posy()+l; j!= para.posy() ; j+=l) {
		if(gameboard->getPlayer(Position(pos.posx(),j)) != -1)
			return false;
	}
	return true;
}
std::vector <Position> *Chess::getVerticalandHorizontal(const Position& p) const {
	std::vector <Position> *ans=new std::vector <Position>;
	int py[]={1,-1,0,0};
	int px[]={0,0,1,-1};
	for(int k=0;k<4;k++) {
		for(int i=(p.posy()+py[k]),j=(p.posx()+px[k]);(i<this->nlines) and (i>=0) and (j < this->ncolums) and (j>=0);i+=py[k],j+=px[k]) {
			Position aux(j,i);
			if(this->gameboard->getPlayer(aux) == -1)
				ans->push_back(aux);
			else break;
		}
	}
	return ans;
}

std::vector <Position> *Chess::getDiagonals(const Position& p) const {
	std::vector <Position> *ans = new std::vector <Position>;
	int dx[]={1,-1,1,-1};
	int dy[]={1,1,-1,-1};
	for(int k=0;k<4;k++) {
		for(int i=(p.posy()+dy[k]),j=(p.posx()+dx[k]);(i<this->ncolums) and(i>=0) and (j < this->ncolums) and (j>=0);i+=dy[k],j+=dx[k]) {
			Position aux(j,i);
			if( this->gameboard->getPlayer(aux) == -1)
				ans->push_back(aux);
			else break;
		}
	}
	return ans;
}

std::vector <Position> *Chess::getPositions(const Position& onde) const {
	int jogador = this->gameboard->getPlayer(onde);
	std::vector <Position> *pos = this->getpossiblePositions(onde);
	std::vector <Position> *ans = new std::vector <Position>;
	for(int i=0;i<(int)pos->size();i++) {
		if(verifyMove( ChessMove(onde,(*pos)[i],jogador) ))
			ans->push_back( (*pos)[i] );
	}
	delete pos;
	return ans;
}

std::vector <Position> *Chess::getpossiblePositions(const Position& onde) const {

	std::vector <Position> *p = new std::vector <Position>;
	switch(this->gameboard->getType(onde)) {
		case 'N':
			{
				int posx[]={1,2,2,1,-1,-2,-2,-1};
				int posy[]={2,1,-1,-2,-2,-1,1,2};
				for(int k=0;k<8;k++)
					p->push_back(Position(posx[k],posy[k])+onde);
			}
			break;
		case 'P':
			{
				int posx[]={0,0,1,-1,0,0,1,-1};
				int posy[]={1,2,1,1,-1,-2,-1,-1};
				for(int k=0;k<8;k++)
					p->push_back(Position(posx[k],posy[k])+onde);
			}
			break;
		case 'R':
			{
				std::vector <Position> *pos = getVerticalandHorizontal(onde);
				p->insert(p->end(),pos->begin(),pos->end());
				delete pos;
			}
			break;
		case 'B':
			{
				std::vector <Position> *pos = getDiagonals(onde);
				p->insert(p->end(),pos->begin(),pos->end());
				delete pos;
			}
			break;
		case 'Q':
			{
				std::vector <Position> *pos = getVerticalandHorizontal(onde);
				p->insert(p->end(),pos->begin(),pos->end());
				delete pos;
				pos = getDiagonals(onde);
				p->insert(p->end(),pos->begin(),pos->end());
				delete pos;
			}
			break;
		case 'K':
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
