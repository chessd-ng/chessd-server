#include "ChessBasedGame.hh"
#include "utils.hh"
ChessBasedGame::ChessBasedGame(int n, int m) : BoardGame(n,m) {
	gameboard=new ChessBoard(n,m);
}

int ChessBasedGame::numberOfTurns() const {
	return this->historico.getHistory().size()-1;
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
					fen+=toString(conta);
				conta=0;
				fen+=(char)((*this->gameboard)[i][j]->pieceReal());
			}
		}
		if(conta>0)
			fen+=toString(conta);
		
		fen+= '/' ;
	}
	fen.erase(fen.end()-1);
	return fen;
}

bool ChessBasedGame::verifyPieceMove(const ChessMove& j) const {
	Position to = j.getto();
	Position from = j.getfrom();
	if((to.posx() >=0) and (to.posx() < this->ncolums) and (to.posy() >= 0) and (to.posy() < nlines)) {
		if((j.getColor()!=(gameboard->getColor(from)))or(j.getColor()==(gameboard->getColor(to))))
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
	Position para = J.getto();
	Position pos = J.getfrom();
	int i,j;
	int k=(para.posx()-pos.posx());k=k/abs(k);
	int l=(para.posy()-pos.posy());l=l/abs(l);
	//nao se preocupa com fora das ordas
	for(i=pos.posx()+k,j=pos.posy()+l;Position(i,j)!=para;i+=k,j+=l)
		if(gameboard->getColor(Position(i,j)) != -1)
			return false;
	return true;
}

bool ChessBasedGame::VerifyHorizontal(const ChessMove& J) const {
	//FIXME
	//sera q precisa fazer a verificacao se e horizontal aqui ou nao?
	Position para = J.getto();
	Position pos = J.getfrom();
	int k=(para.posx()-pos.posx());k=k/abs(k);
	for(int i=pos.posx()+k; i!=para.posx() ; i+=k) {
		//acessa tabuleiro e verifica
		if(gameboard->getColor(Position(i,pos.posy())) != -1)
			return false;
	}
	return true;
}

bool ChessBasedGame::VerifyVertical(const ChessMove& J) const {
	Position para = J.getto();
	Position pos = J.getfrom();
	int l=(para.posy()-pos.posy());l=l/abs(l);
	for(int j=pos.posy()+l; j!= para.posy() ; j+=l) {
		if(gameboard->getColor(Position(pos.posx(),j)) != -1)
			return false;
	}
	return true;
}

bool ChessBasedGame::verifyHorseMove(const ChessMove& j) const {
	int distx,disty;
	distx=abs(j.getfrom().posx()-j.getto().posx());
	disty=abs(j.getfrom().posy()-j.getto().posy());
	if(gameboard->getColor(j.getto()) == j.getColor())
			return false;
	if( distx+disty == 3 )
		if( (distx==2 and disty==1) or (distx==1 and disty==2) )
			return true;
	return false;
}

bool ChessBasedGame::verifyPawnMove(const ChessMove& jogada) const {
	int distx,disty;
	const Position& from = jogada.getfrom();
	const Position& to = jogada.getto();
	distx=abs(from.posx()-to.posx());
	disty=(to.posy()-from.posy());

	if( (jogada.getColor() == 0) && (disty < 0))
		return false;
	else if( (jogada.getColor() == 1) && (disty > 0))
		return false;
	if(gameboard->getColor(to) == jogada.getColor())
		return false;

	disty=abs(disty);

	//Pawn moving just one square
	if( (disty == 1) and (distx == 0) ) {
		if(gameboard->getColor(to) != -1)
			return false;
		return true;
	}
	//Pawn moving two squares ahead
	else if( (disty == 2) and (distx == 0) ) {
		int meio=(from.posy()+to.posy())/2;
		if(gameboard->getColor(Position(from.posx(),meio)) != -1)
			return false;
		if(gameboard->getColor(to) != -1)
			return false;

		//FIXME pouco portavel
		//seria bom gaurdar a posicao inicial y de cada peao
		if(gameboard->getColor(from) == 0) {
			if(from.posy() != 1)
				return false;
		}
		else 
			if(from.posy() != this->nlines - 2)
				return false;
		return true;
	}
	else if( (disty == 1) and (distx == 1) ) {
//		if( (gameboard->getColor(to) == -1) and (gameboard->getColor(Position(from.posx(),to.posy())) != (jogada.getColor()+1)%2 ) )
		if( (this->gameboard->getColor(to) == -1) ) {
			if( this->atual.enpassant == to)
				return true;
			else
				return false;
		}
		else if(this->gameboard->getColor(to) != jogada.getColor())
			return true;			
	}
	else
		return false;

	return true;
}

bool ChessBasedGame::verifyRookMove(const ChessMove& j) const {
	int distx,disty;
	distx=abs(j.getfrom().posx()-j.getto().posx());
	disty=abs(j.getfrom().posy()-j.getto().posy());
//	const Position& from=j.getfrom();
//	const Position& to=j.getto();
	if(gameboard->getColor(j.getto()) != j.getColor())
	{
		if(gameboard->getColor(j.getto()) != j.getColor())
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
	const Position& to = J.getto();
	const Position& from = J.getfrom();
	distx=abs(from.posx()-to.posx());
	disty=abs(from.posy()-to.posy());
	if(gameboard->getColor(J.getto()) != J.getColor())
		if( (distx == disty) and (distx!=0) ) 
			return VerifyDiagonal(J);
	
	return false;
}

bool ChessBasedGame::verifyQueenMove(const ChessMove& J) const {
	int distx,disty;
	distx=abs(J.getfrom().posx()-J.getto().posx());
	disty=abs(J.getfrom().posy()-J.getto().posy());

	if(gameboard->getColor(J.getto()) != J.getColor()) {
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
	if(gameboard->getColor(J.getto()) != J.getColor()) {
		int distx=(J.getfrom().posx()-J.getto().posx());
		int disty =(J.getfrom().posy()-J.getto().posy());
		if( (abs(distx) <=1) and ( abs(disty) <= 1))
			return true;
		if( ( abs(distx) == 2) and ( abs(disty) == 0) ) {
			char rainha='q',rei='k';
			if(J.getColor() == WHITE) {
				rainha='Q';	rei='K';
			}
			//foi castle
			if((distx > 0 and (atual.castle.find(rainha,0) < atual.castle.size())) or ((distx<0)and(atual.castle.find(rei,0) < atual.castle.size()))) {
				//verifica se tem alguem entre a torre e o rei
				if(verifyRookMove(ChessMove(distx>0?Position(0,J.getto().posy()):Position(7,J.getto().posy()),Position(J.getfrom().posx()-distx/abs(distx),J.getto().posy()),J.getColor()))==false)
					return false;
				if(verifyCheck(J.getColor())) //se o rei tah em cheque
					return false;
				else if(beingAttacked(Position(J.getfrom().posx()+J.getto().posx()/2,J.getto().posy()),J.getColor()^1)) //se 1 depois da posicao do rei tah sendo atacada
					return false;
				else if(beingAttacked(J.getto(),J.getColor()^1)) //se 2 depois da posicao do rei tah sendo atacada
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
			if(this->gameboard->getColor(Position(j,i)) == jogador)
				if(this->verifyPieceMove(ChessMove(Position(j,i),onde,jogador)))
					return true;
	return false;
}

Position ChessBasedGame::findKing(int player) const {
	for(int i=0; i<this->nlines;i++)
		for(int j=0;j<this->ncolums;j++)
			if( (this->gameboard->getType(Position(j,i)) == ChessPiece::KING) and (this->gameboard->getColor(Position(j,i)) == player) )
				return Position(j,i);
	return Position(-1,-1);
}

//this function does not verify anything
bool ChessBasedGame::verifyEnPassant(const ChessMove& j) const {
	if(this->gameboard->getType(j.getfrom()) == ChessPiece::PAWN) {
		int distx=abs(j.getto().posx() - j.getfrom().posx() );
		int disty=abs(j.getto().posy() - j.getfrom().posy() );
		if( (distx == 1) and (disty == 1) and (this->gameboard->getColor(j.getto()) == -1) ) {
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
		for(int i=(p.posy()+py[k]),j=(p.posx()+px[k]);(i<this->nlines) and (i>=0) and (j < this->ncolums) and (j>=0);i+=py[k],j+=px[k]) {
			Position aux(j,i);
			if(this->gameboard->getColor(aux) == -1)
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
		for(int i=(p.posy()+dy[k]),j=(p.posx()+dx[k]);(i<this->ncolums) and(i>=0) and (j < this->ncolums) and (j>=0);i+=dy[k],j+=dx[k]) {
			Position aux(j,i);
			if( this->gameboard->getColor(aux) == -1)
				ans->push_back(aux);
			else break;
		}
	}
	return ans;
}

std::vector <Position> *ChessBasedGame::getPositions(const Position& onde) const {
	int jogador = this->gameboard->getColor(onde);
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
