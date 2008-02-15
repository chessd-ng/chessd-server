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

	this->_title=this->_players[0].jid.full()+" x "+this->_players[1].jid.full();

	this->_resign=Chess::UNDEFINED;
	this->_draw=false;
	colormap[this->_teams[0][0]]=_players[0].color==White?Chess::WHITE:Chess::BLACK;
	colormap[this->_teams[1][0]]=_players[1].color==White?Chess::WHITE:Chess::BLACK;

	//FIXME
	//centralize the history
	history_saved.openTag("game");
	history_saved.addAttribute("category",this->category());
	history_saved.addChild(GameChess::generateStateTag(chess.getState(),Util::Time() ));
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
			if( ( chess.turn()==(colormap.find(it->jid)->second) ) and (chess.numberOfTurns() > 2))
				aux-=current_time-time_of_last_move;
			
			//XXX be careful with double from getSeconds
			t.addAttribute("time", Util::to_string((int)aux.getSeconds()));

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
}

void GameChess::call_flag(const Player& player) {
	//TODO the time will come =D
}

void GameChess::draw() {
	this->_draw=true;
}

void GameChess::adjourn() {
	//TODO
}

bool GameChess::done(void) const {
	foreach(it,standard_player_map)
		if(it->second->time <= Util::Time())
			return true;
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
			return std::string("Time of ")+it->second->jid.full()+std::string(" has ended");
	}
	if(chess.verifyCheckMate())
		return "Checkmate";
	else if(this->_resign!=Chess::UNDEFINED)
		return (this->_resign==Chess::WHITE?_teams[0][0].full():_teams[1][0].full())+std::string(" resigned");
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
	if((this->standard_player_map[player]->time)-time_stamp+time_of_last_move < Util::Time())
		throw time_over("Time has ended");

	if(colormap[player]!=chess.turn())
		throw wrong_turn(std::string("It's not ")+player.full()+std::string("'s turn"));

	if(chess.verifyAndMakeMove(movement)==false)
		throw invalid_move("Invalid Move");

	if(chess.numberOfTurns() > 2)
		this->standard_player_map[player]->time-=time_stamp-time_of_last_move-this->standard_player_map[player]->inc;

	time_of_last_move=time_stamp;

	this->history_saved.addChild(this->generateStateTag(chess.getState(),time_stamp));
	if(done()==true)
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
