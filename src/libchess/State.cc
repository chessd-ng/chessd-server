#include "State.hh"
#include <string>
#include <iostream>

std::string State::toString(int n) {
	char tmp[16];
	sprintf(tmp,"%d",n);
	return std::string(tmp);
}

State::State() {
	this->castle=std::string("KQkq");
	this->enpassant=Position(-1,-1);
	this->lastenpassant=Position(-1,-1);
	this->halfmoves=0;
	this->fullmoves=1;
	this->vez=BRANCAS;
	//cuidado com isso
	this->tabfen=std::string("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}
State::State(const std::string posfen) {
	this->tabfen=posfen;
	this->castle=std::string("KQkq");
	this->enpassant=Position(-1,-1);
	this->lastenpassant=Position(-1,-1);
	this->halfmoves=0;
	this->fullmoves=1;
	this->vez=BRANCAS;
}
State::State(const State& a) {
	*this=a;
}
std::string State::getFEN() const {
	return std::string(tabfen);
}
std::string State::getentireFEN() const {
	std::string ans;
	ans=this->tabfen;
	ans+=" ";
	ans+=(char)(this->vez == BRANCAS ? 'w' : 'b');
	ans+=" ";
	ans+=this->castle;
	ans+=" ";
	if( this->enpassant == Position(-1,-1) )
		ans+='-';
	else
		ans+=this->enpassant.toStringNotation();
	ans+=" ";
	ans+=toString(this->halfmoves);
	ans+=" ";
	ans+=toString(this->fullmoves);
	return ans;
}

History::History() {
	this->allgame.clear();
}

History::History(const State& est) {
	this->allgame.clear();
	this->putinHistory(est);
}

History::~History() {
	allgame.clear();
}

void History::putinHistory(const State& est) {
	allgame.push_back(est);
}

const std::vector<State>& History::getHistory() const {
	return this->allgame;
}

#ifdef TESTE
void History::Desenha() const {
	for(int i=0;i<(int)allgame.size();i++) {
		std::cout << allgame[i].getentireFEN() << std::endl;
	}
}
#endif
