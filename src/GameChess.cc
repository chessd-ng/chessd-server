/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

#include "GameChess.hh"
#include <iostream>
#include "Util/utils.hh"
#include <string>
#include "GameException.hh"
//TODO
//tempo
//testar XML

GameChess::GameChess(const StandardPlayerList& _players, const std::string &_category) {
	this->_category=_category;
	foreach(it,_players) {
		Team a;
		a.push_back(it->jid);
		this->_teams.push_back(a);
	}
	this->_players=_players;

	this->_title=this->_players[0].jid.full()+" x "+this->_players[1].jid.full();

	this->_resign=Chess::UNDEFINED;
	this->_draw=false;
	colormap[this->_teams[0][0]]=_players[0].color==White?Chess::WHITE:Chess::BLACK;
	colormap[this->_teams[1][0]]=_players[1].color==White?Chess::WHITE:Chess::BLACK;
}

XML::Tag* GameChess::generateStateTag(const State &est) {
	XML::TagGenerator t;
	t.openTag("board");
	{
		t.addAttribute("state",est.getFEN());
		t.addAttribute("turn",est.vez==Chess::WHITE?"w":"b");
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

const std::string& GameChess::category() const {
	return this->_category;
}

const std::string& GameChess::title() const {
	return this->_title;
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

	if((checkmate=chess.verifyCheckMate()) or this->_resign!=Chess::UNDEFINED) {
		reason=(checkmate==true)?"Checkmate":"The other player resigned";
		if(this->_resign==Chess::BLACK or (chess.winner()==Chess::WHITE)) {
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
<<<<<<< HEAD:src/GameChess.cc
	if(colormap[player]!=chess.Turn())
		throw wrong_turn(std::string("It's not ")+player.full()+std::string(" turn"));
	if(chess.verifyandmakeMove(movement)==false)
		throw invalid_move("Invalid Move");
=======
	if(colormap[player]!=chess.turn())
		throw "It's not your turn";
	if(chess.verifyAndMakeMove(movement)==false)
		throw "Invalid Move";
>>>>>>> libchess-experiment:src/GameChess.cc
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

    /* Raphael: playerslist was empty */
    foreach(team, l)
        foreach(player, team->first)
            this->playerlist.push_back(*player);
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
