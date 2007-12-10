#ifndef __JOGADAH__
#define __JOGADAH__
#include <string>
struct Position
{
	int pos[2];

	int posx() const ;
	int posy() const ;
	std::string toStringNotation() const ;

	Position(std::string& p);
	Position(int x, int y);
	Position() ; 

	//ver se n precisa copiar
	bool operator ==(const Position& p) const ;
	bool operator !=(const Position& p) const ;
	Position operator +(const Position& p) const {
		return Position(this->posx()+p.posx(),this->posy()+p.posy());
	}
};

class ChessMove
{
	private:
		Position from;
		Position to;
		int jogador;
	public:
		ChessMove();
		ChessMove(int jogador,const std::string&);
		
		ChessMove(const Position& a, const Position& b,const int j);

		Position getto() const ;
		Position getfrom() const ;
		int getPlayer() const ;
};

#endif
