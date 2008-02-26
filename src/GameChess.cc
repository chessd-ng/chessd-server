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
#include "HistoryProcess.hh"

GameChess::GameChess(const StandardPlayerList& _players, const std::string &_category) : time_of_last_move() {
	this->_category=_category;
	this->_players=_players;

	foreach(it,_players)
		this->_teams.push_back(Team(1,it->jid));

	foreach(it,this->_players)
		standard_player_map[it->jid]=&(*it);

	this->colormap[this->_teams[0][0]]=_players[0].color==White?Chess::WHITE:Chess::BLACK;
	this->colormap[this->_teams[1][0]]=_players[1].color==White?Chess::WHITE:Chess::BLACK;

	if(_players[0].color==White)
		this->_title=this->_players[0].jid.partial()+" x "+this->_players[1].jid.partial();
	else
		this->_title=this->_players[1].jid.partial()+" x "+this->_players[0].jid.partial();

	this->initial_time=_players[0].time.getSeconds()+0.001;

	//These three are set only for ending reasons
	this->_resign=Chess::UNDEFINED;
	this->time_over=-1;
	this->_done=NOREASON;
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
		if(est.enpassant.x!=-1)
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
			if( ( chess.turn()==(colormap.find(it->jid)->second) ) and (chess.numberOfTurns() >= 2) and (this->_done==NOREASON))
				aux-=current_time-time_of_last_move;
			
			//XXX be careful with double from getSeconds
			t.addAttribute("time", Util::to_string((int)(std::max(aux.getSeconds()+0.001,0.0))));

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

XML::Tag* GameChess::history() const {
	XML::Tag* h=generateHistoryTag();
	ChessHistoryProcess chp;
	return chp.generate(h);
}

const std::string& GameChess::category() const {
	return this->_category;
}

const std::string& GameChess::title() const {
	return this->_title;
}

void GameChess::resign(const Player& player) {
	this->_resign=colormap[player];
	this->_done=RESIGNED;
}

void GameChess::call_flag(const Player& player) {
}

void GameChess::draw() {
	this->_done=DRAWAGREED;
}

void GameChess::adjourn() {
	//TODO
}

bool GameChess::done(const Util::Time& current_time) {
	if(this->_done==NOREASON) {
		if(this->chess.numberOfTurns() >= 2)
			foreach(it,standard_player_map)
				if(it->second->time+(this->colormap[it->first]==this->chess.turn()?this->time_of_last_move-current_time:Util::Time()) <= Util::Time()) {
					this->time_over=it->second->color;
					this->_done=TIMEOVER;
				}
	}
	return this->_done!=0;
}

int GameChess::realDone() {
	int aux=0;
	if(this->_done!=0)
		return this->_done;
	else if(chess.verifyCheckMate())
		return CHECKMATE;
	else if((aux=chess.verifyDraw())!=0)
		return aux+4;
	return 0;
}

std::string GameChess::doneEndReason() const {
	std::string reason;
	switch(this->_done) {
		case RESIGNED:
			return std::string(this->_resign==Chess::WHITE?"White":"Black")+std::string(" has resigned");
			break;
		case CHECKMATE:
			return std::string(this->chess.turn()==Chess::WHITE?"Black":"White")+std::string(" has won by checkmate");
			break;
		case DRAWAGREED:
			return "The players agreed on a draw";
			break;
		case TIMEOVER:
			return std::string("Time of ")+std::string(time_over==int(White)?"white":"black")+std::string(" has ended");
			break;
		case DRAWREPETITION:
			return "Draw by three fold repetition rule";
			break;
		case DRAWIMPOSSIBILITYOFCHECKMATE:
			return "Draw by impossibility of checkmate";
			break;
		case DRAWFIFTYMOVE:
			return "Draw by fifty move rule";
			break;
		case DRAWSTALEMATE:
			return "Draw by stalemate";
			break;
		default:
			break;
	}
	return "";
}

PlayerResultList GameChess::donePlayerResultList() const {
	/* prl is indexed for
	 * 0 is white
	 * 1 is black*/
	PlayerResultList prl(2);
	foreach(it,_players) {
		prl[colormap.find(it->jid)->second]=PlayerResult(it->jid,(it->color==White?"white":"black"),(""));
	}
	if(this->_done==3 or this->_done>=5)
		prl[0].score=prl[1].score="1/2";
	else if (this->_done!=NOREASON) {
		bool aux=this->_resign==Chess::BLACK or (chess.winner()==Chess::WHITE) or (this->time_over==Black);
		prl[0].score=aux==true?"1":"0";
		prl[1].score=aux==true?"0":"1";
	}
	return prl;
}

void GameChess::move(const Player& player, const std::string& movement, const Util::Time& time_stamp) {
	if(this->_done!=0)
		return;

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

	this->_done=(end_reason)realDone();

	history_moves+=movement+" "+Util::to_string(int(this->standard_player_map[player]->time.getSeconds()+0.001))+" ";
}

XML::Tag* GameChess::generateHistoryTag() const {
	XML::TagGenerator gen;

	gen.openTag("history");
	gen.addAttribute("category",this->category());
	{
		gen.openTag("moves");
		{
			gen.addAttribute("movetext",std::string(this->history_moves.begin(),this->history_moves.end()-1));
			gen.closeTag();
		}
		PlayerResultList prl=this->donePlayerResultList();
		foreach(it,prl) {
			gen.openTag("player");

			gen.addAttribute("jid",it->jid.full());

			gen.addAttribute("color",it->role);

			gen.addAttribute("score",it->score);

			gen.addAttribute("time",Util::to_string(this->initial_time));

			gen.closeTag();
		}
		gen.closeTag();
	}
	return gen.getTag();
}

const TeamList& GameChess::teams() const {
	return this->_teams;
}
//--------------------------------------
//CHESS GAME RESULT Stuff
//--------------------------------------

ChessGameResult::ChessGameResult(const std::string &endreason,const PlayerResultList &prl, const std::string& __category, XML::Tag* hist) : _history(hist) {
	this->_end_reason=endreason;
	this->player_result_list=prl;
	this->_category=__category;
}

const std::string& ChessGameResult::category() const {
	return this->_category;
}

const std::string& ChessGameResult::end_reason() const {
	return this->_end_reason;
}

const PlayerResultList& ChessGameResult::players() const {
	return this->player_result_list;
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
	foreach(it,this->player_result_list) {
		playerlist.push_back(it->jid);
		playerrating.push_back(ratings[it->jid]);
	}

	int i=0,j=1;
	foreach(it,player_result_list) {
		//TODO
		//Do Step 1 of glicko System that it determines the new RD(Rating deviation)
		//that is based on the difference between now and the date since the last competition
		//in seconds
		//Just need to receive the last time the player has played
		double w;
		if(it->score == "1") {
			playerrating[i].wins()++;
			w=1.0;
		}
		else if(it->score == "1/2") {
			playerrating[i].draws()++;
			w=0.5;
		}
		else { //it->score = "0"
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
