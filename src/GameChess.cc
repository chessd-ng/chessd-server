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

#include <string>
#include <cmath>
#include "GameChess.hh"
#include "Util/utils.hh"
#include "GameException.hh"
#include "GlickoSystem.hh"
//TODO
//tempo
//testar XML

GameChess::GameChess(const StandardPlayerList& _players, const std::string &_category) : time_of_last_move() {
	this->_category=_category;
	foreach(it,_players) {
		Team a;
		a.push_back(it->jid);
		this->_teams.push_back(a);
	}
	this->_players=_players;
	foreach(it,this->_players) {
		standard_player_map[it->jid]=&(*it);
	}

	this->_title=this->_players[0].jid.partial()+" x "+this->_players[1].jid.partial();

	this->_resign=Chess::UNDEFINED;
	this->_draw=false;
	this->colormap[this->_teams[0][0]]=_players[0].color==White?Chess::WHITE:Chess::BLACK;
	this->colormap[this->_teams[1][0]]=_players[1].color==White?Chess::WHITE:Chess::BLACK;

	//FIXME
	//centralize the history
	this->history_saved.openTag("game");
	this->history_saved.addAttribute("category",this->category());
	this->history_saved.addChild(GameChess::generateStateTag(chess.getState(),Util::Time()));

	this->_history=0;
}

XML::Tag* GameChess::generateStateTag(const ChessState &est, const Util::Time& current_time) const {
	XML::TagGenerator t;
	t.openTag("state");
	t.addAttribute("category",this->category());

	t.openTag("board");
	{
		t.addAttribute("state",est.boardFEN());
		t.addAttribute("turn",est.turn()==Chess::WHITE?"white":"black");
		t.addAttribute("castle",est.castle);
		if(est.enpassant.x()!=-1)
			t.addAttribute("enpassant",est.enpassant.toStringNotation());
		else
			t.addAttribute("enpassant","-");
		t.addAttribute("halfmoves",Util::to_string(est.halfmoves));
		t.addAttribute("fullmoves",Util::to_string(est.fullmoves));
	}
	t.closeTag();
	foreach(it,this->_players) {
		t.openTag("player");
		{   
			t.addAttribute("jid",it->jid.full());
			Util::Time aux=it->time;
			if( ( chess.turn()==(colormap.find(it->jid)->second) ) and (chess.numberOfTurns() >= 2))
				aux-=current_time-time_of_last_move;
			
			//XXX be careful with double from getSeconds
			t.addAttribute("time", Util::to_string((int)(aux.getSeconds()+0.001)));

			t.addAttribute("color",it->color==White?"white":"black");
			t.closeTag();
		}
	}
	t.closeTag();
	return t.getTag();
}

XML::Tag* GameChess::state(const Util::Time& current_time) const {
	return (generateStateTag(chess.getState(),current_time));
}

const std::string& GameChess::category() const {
	return this->_category;
}

const std::string& GameChess::title() const {
	return this->_title;
}

void GameChess::resign(const Player& player) {
	this->_resign=colormap[player];
	_history=history_saved.getTag();
}

void GameChess::call_flag(const Player& player) {
	//TODO the time will come =D
}

void GameChess::draw() {
	this->_draw=true;
	_history=history_saved.getTag();
}

void GameChess::adjourn() {
	//TODO
}

bool GameChess::done(const Util::Time& current_time) {
	foreach(it,standard_player_map)
		if(it->second->time+this->time_of_last_move-current_time <= Util::Time()) {
			it->second->time+=this->time_of_last_move-current_time;
			if(this->_history==0)
				this->_history=history_saved.getTag();
			return true;
		}
	if(chess.verifyCheckMate() or this->_resign!=Chess::UNDEFINED)
		return true;
	else if(this->_draw==true)
		return true;
	else if(chess.verifyDraw()==true)
		return true;
	return false;
}

std::string GameChess::doneEndReason() const {
	std::string reason;
	foreach(it,standard_player_map) {
		if(it->second->time <= Util::Time())
			return std::string("Time of ")+std::string(it->second->color==White?"white":"black")+std::string(" has ended");
	}
	if(chess.verifyCheckMate())
		return "Checkmate";
	else if(this->_resign!=Chess::UNDEFINED)
		return std::string(this->_resign==Chess::WHITE?"white":"black")+std::string(" has resigned");
	else if(this->_draw==true)
		return "The players agreed on a draw";
	else if(chess.verifyDraw()==true)
		return "Draw";
	return "";
}

TeamResultList GameChess::doneTeamResultList() const {
	TeamResultList trl;
	for(int i=0;i<(int)_teams.size();i++)
		trl.push_back(make_pair(_teams[i],UNDEFINED));
	foreach(it,standard_player_map) {
		if(it->second->time <= Util::Time()) {
			if(it->second->color==White) {
				trl[0].second=LOSER;
				trl[1].second=WINNER;
			} else {
				trl[0].second=WINNER;
				trl[1].second=LOSER;
			}
		}
	}
	if(trl[0].second==UNDEFINED) {
		if(chess.verifyCheckMate() or (this->_resign!=Chess::UNDEFINED)) {
			if(this->_resign==Chess::BLACK or (chess.winner()==Chess::WHITE)) {
				trl[0].second=WINNER;
				trl[1].second=LOSER;
			}
			else {
				trl[0].second=LOSER;
				trl[1].second=WINNER;
			}
		}
		else if(this->_draw==true)
			trl[0].second=trl[1].second=DRAWER;
		else if(chess.verifyDraw()==true)
			trl[0].second=trl[1].second=DRAWER;
	}

	return trl;
}

void GameChess::move(const Player& player, const std::string& movement, const Util::Time& time_stamp) {
	if(chess.numberOfTurns() >= 2) {
		if((this->standard_player_map[player]->time)-time_stamp+time_of_last_move < Util::Time())
			return;
		this->standard_player_map[player]->time-=time_stamp-time_of_last_move-this->standard_player_map[player]->inc;
	}

	if(colormap[player]!=chess.turn())
		throw wrong_turn(std::string("It's not ")+std::string(colormap[player]==Chess::WHITE?"white":"black")+std::string("'s turn"));

	if(chess.verifyAndMakeMove(movement)==false)
		throw invalid_move("Invalid Move");

	time_of_last_move=time_stamp;

	this->history_saved.addChild(this->generateStateTag(chess.getState(),time_stamp));
	if(this->done(time_stamp)==true)
		_history=history_saved.getTag();
}

const TeamList& GameChess::teams() const {
	return this->_teams;
}
//--------------------------------------
//CHESS GAME RESULT Things
//--------------------------------------

ChessGameResult::ChessGameResult(const std::string &endreason,const TeamResultList &l, const std::string& _category, XML::Tag* hist) : _history(hist) {
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
	return this->_history;
}

void ChessGameResult::updateRating(std::map<Player, Rating> &ratings) const {
	foreach(it,ratings) {
		if(it->second.countGames() == 0) {
			it->second.rating()=1500;
			it->second.volatility()=350.0;
		}
	}
	std::vector<Player> playerlist;
	std::vector<Rating> playerrating;
	foreach(it,teamresultlist) {
		playerlist.push_back(it->first[0]);
		playerrating.push_back(ratings[it->first[0]]);
	}

	int i=0,j=1;
	foreach(it,teamresultlist) {
		//TODO
		//Do Step 1 of glicko System that it determines the new RD(Rating deviation)
		//that is based on the difference between now and the date since the last competition
		//in seconds
		//Just need to receive the last time the player has played
		double w;
		if(it->second == WINNER) {
			playerrating[i].wins()++;
			w=1.0;
		}
		else if(it->second == DRAWER) {
			playerrating[i].draws()++;
			w=0.5;
		}
		else { //it->second = LOSER
			playerrating[i].losses()++;
			w=0.0;
		}
		double E=GlickoSystem::E(ratings[playerlist[i]],ratings[playerlist[j]]);

		double fs2=GlickoSystem::gRD(ratings[playerlist[j]]);

//		economical way
		double denominator=1.0/(GlickoSystem::square(ratings[playerlist[i]].volatility())) + GlickoSystem::square(GlickoSystem::q()) * GlickoSystem::square(fs2) * E * (1.0 - E);

//		Pratical way
//		double denominator=1.0/(GlickoSystem::square(ratings[playerlist[i]].volatility())) + 1.0/GlickoSystem::dsquare(ratings[playerlist[i]],ratings[playerlist[j]]);

		playerrating[i].rating()+=GlickoSystem::round(GlickoSystem::q()*fs2*(w-E)/denominator);

		playerrating[i].volatility()=1.0/sqrt(denominator);
		i++;
		j=(j+1)%2;
	}
	for(i=0;i<(int)playerrating.size();i++) {
		ratings[playerlist[i]]=playerrating[i];
	}
}
