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

	this->_title=this->_players[0].jid.full()+" x "+this->_players[1].jid.full();

	this->_resign=NENHUM;
	this->_draw=false;
	colormap[this->_teams[0][0]]=_players[0].color==White?BRANCAS:PRETAS;
	colormap[this->_teams[1][0]]=_players[1].color==White?BRANCAS:PRETAS;
}

XML::Tag* GameChess::generateStateTag(const State &est) {
	XML::TagGenerator t;
	t.openTag("board");
	{
		t.addAttribute("state",est.getFEN());
		t.addAttribute("turn",est.vez==BRANCAS?"w":"b");
		t.addAttribute("castle",est.castle);
		if(est.enpassant.posx()!=-1)
			t.addAttribute("enpassant",est.enpassant.toStringNotation());
		else
			t.addAttribute("enpassant","-");
		t.addAttribute("halfmoves",State::toString(est.halfmoves));
		t.addAttribute("fullmoves",State::toString(est.fullmoves));
	}
	t.closeTag();
	return t.getTag();
}
XML::Tag* GameChess::state() const {
	XML::TagGenerator t;
	t.openTag("game");
	t.addAttribute("category",this->category());

	XML::Tag *tag=generateStateTag(chess.getState());
	t.addChild(tag);

	t.closeTag();
	return t.getTag();
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
	//TODO
}
GameResult* GameChess::done(void) const {
	TeamResultList l;
	for(int i=0;i<(int)_teams.size();i++)
		l.push_back(make_pair(_teams[i],UNDEFINED));

	bool checkmate;
	std::string reason;

	if((checkmate=chess.verifyCheckMate()) or this->_resign!=color(NENHUM)) {
		reason=(checkmate==true)?"Checkmate":"The other player resigned";
		if(this->_resign==color(PRETAS) or (chess.Winner()==0)) {
			l[0].second=WINNER;
			l[1].second=LOSER;
		}
		else {
			l[1].second=WINNER;
			l[0].second=LOSER;
		}
	}
	else if(this->_draw==true) {
		l[0].second=l[1].second=DRAWER;
		reason="The players agreed on a draw";
	}
	else if(chess.verifyDraw()==true) {
		l[0].second=l[1].second=DRAWER;
		reason="Draw";
	}

	if(reason.size()!=0)
		return this->newGameResult(reason,l,chess.getHistory());
	return NULL;
}

void GameChess::move(const Player& player, const std::string& movement) {
	if(colormap[player]!=chess.Turn())
		throw "It's not your turn";
	if(chess.verifyandmakeMove(movement)==false)
		throw "Invalid Move";
}

const TeamList& GameChess::teams() const {
	return this->_teams;
}
//--------------------------------------
//CHESS GAME RESULT Things
//--------------------------------------

ChessGameResult::ChessGameResult(const std::string &endreason,const TeamResultList &l, const std::string& _category, const std::vector<State> &st) : states(st){
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
	XML::TagGenerator t;
	t.openTag("game");
	t.addAttribute("category",this->category());
	foreach(it,states) {
		t.addChild(GameChess::generateStateTag(*it));
	}
	t.closeTag();
	return t.getTag();
}
