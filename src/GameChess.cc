#include "GameChess.hh"
#include <iostream>
#include "Util/utils.hh"
//TODO
//tempo
//testar XML

GameChess::GameChess(const StandardPlayerList& _players) {
	foreach(it,_players) {
		Team a;
		a.push_back(it->jid);
		this->_teams.push_back(a);
	}

	this->_players=_players;
//	this->_title=this->_players[0].jid.node()+" x "+this->_players[1].jid.node();

	//FIXME zuado!
	this->_title=this->_players[0].jid.node()+"@"+this->_players[0].jid.domain()+" x "+this->_players[1].jid.node()+"@"+this->_players[1].jid.domain();

	this->_resign=NENHUM;
	this->_draw=false;
	colormap[this->_teams[0][0]]=_players[0].color==White?BRANCAS:PRETAS;
	colormap[this->_teams[1][0]]=_players[1].color==White?BRANCAS:PRETAS;
}

XML::Tag* GameChess::state() const {
	const State est=chess.getState();
	XML::TagGenerator t;
	t.openTag("game");
	t.addAttribute("category",this->category());
	t.openTag("board");
	t.addAttribute("state",est.getFEN());
	t.addAttribute("turn",chess.vez==BRANCAS?"w":"b");
	t.addAttribute("castle",est.castle);
	if(est.enpassant.posx()!=-1)
		t.addAttribute("enpassant",est.enpassant.toStringNotation());
	else
		t.addAttribute("enpassant","-");
	t.addAttribute("halfmoves",State::toString(est.halfmoves));
	t.addAttribute("fullmoves",State::toString(est.fullmoves));
	t.closeTag();
	return t.closeTag();
}
const std::string& GameChess::title() const {
	return _title;
}
void GameChess::resign(const Player& player) {
	this->_resign=colormap[player];
}
void GameChess::call_flag(const Player& player) {
	//TODO o tempo virá, tenha calma =D
}
void GameChess::draw() {
	this->_draw=true;
}
void GameChess::adjourn() {
	//sei lah
}
GameResult* GameChess::done(void) const {
	GameResult *result=NULL;
	TeamResultList l;
	for(int i=0;i<(int)_teams.size();i++)
		l.push_back(make_pair(_teams[i],UNDEFINED));

	bool checkmate;
	if((checkmate=chess.verifyCheckMate()) or this->_resign!=color(NENHUM)) {
		std::string reason;
		reason=(checkmate==true)?"Checkmate":"The other player resigned";
		if(this->_resign!=NENHUM or (chess.Winner()==0)) {
			l[0].second=WINNER;
			l[1].second=LOSER;
			result = new ChessGameResult(reason,l,this->category());
		}
		else {
			l[1].second=WINNER;
			l[0].second=LOSER;
			result = new ChessGameResult(reason,l,this->category());
		}
	}
	else if(this->_draw==true) {
		l[0].second=l[1].second=DRAWER;
		result = new ChessGameResult(std::string("The players agreed on a draw"),l,this->category());
	}
	else if(chess.verifyDraw()==true) {
		l[0].second=l[1].second=DRAWER;
		result = new ChessGameResult(std::string("Draw"),l,this->category());
	}
	return result;
}

void GameChess::move(const Player& player, const std::string& movement) {
	if(colormap[player]!=chess.Turn())
		throw std::string("It's not your turn");
	if(chess.verifyandmakeMove(movement)==false)
		throw std::string("Invalid Move");
}

const TeamList& GameChess::teams() const {
	return this->_teams;
}
//--------------------------------------
//COISAS DO CHESS GAME RESULT
//--------------------------------------

ChessGameResult::ChessGameResult(const std::string &endreason,const TeamResultList &l, const std::string& _category) {
	this->_end_reason=endreason;
	this->teamresultlist=l;
	this->_category=_category;
}
const std::string& ChessGameResult::category() const {
	return this->_category;
}
const std::string& ChessGameResult::end_reason() const {
	return this->_end_reason;
}
const PlayerList& ChessGameResult::players() const {
	return this->playerlist;
}
const TeamResultList& ChessGameResult::results() const {
	return this->teamresultlist;
}

XML::Tag* ChessGameResult::history() const {
	//TODO
	return NULL;
}
void ChessGameResult::updateRating(std::map<Player,Rating> &ratings) const {
	//TODO
	//nao sei o q fazer
}
