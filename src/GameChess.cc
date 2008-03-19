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

	/*
	 * 0 is white
	 * 1 is black
	*/
	if(this->_players[0].color==Black)
		std::swap(this->_players[0],this->_players[1]);

	foreach(it,this->_players) {
		standard_player_map[it->jid]=&(*it);
        _simple_players.push_back(it->jid);
    }

	this->colormap[this->_players[0].jid]=this->_players[0].color==White?Chess::WHITE:Chess::BLACK;
	this->colormap[this->_players[1].jid]=this->_players[1].color==White?Chess::WHITE:Chess::BLACK;

	this->_title=this->_players[0].jid.node()+" x "+this->_players[1].jid.node();

	this->initial_time=int(this->_players[0].time.getSeconds()+0.001);

	//These three are set only for ending reasons
	this->_resign=Chess::UNDEFINED;
	this->time_over=-1;
	this->_done=NOREASON;
	this->turns_restart=0;
}

GameChess::GameChess(XML::Tag* adjourned_game) {
	this->_category=adjourned_game->getAttribute("category");

	foreach(it,adjourned_game->tags()) {
		if(it->name()=="player") {
			this->_players.push_back(StandardPlayer(XMPP::Jid(it->getAttribute("jid")),
						Util::Time(it->getAttribute("time_left"),Util::Seconds),
						Util::Time(it->getAttribute("inc"),Util::Seconds),
						StandardPlayerColor(it->getAttribute("color")=="white"?White:Black)));
			this->colormap[this->_players.rbegin()->jid]=this->_players.rbegin()->color==White?Chess::WHITE:Chess::BLACK;
			this->initial_time=Util::parse_string<int>(it->getAttribute("time"));
        	_simple_players.push_back(XMPP::Jid(it->getAttribute("jid")));
		}
		else {
			this->history_moves=it->getAttribute("movetext")+" ";
		}
	}
	/*
	 * 0 is white
	 * 1 is black
	 */
	if(this->_players[0].color==Black)
		std::swap(this->_players[0],this->_players[1]);

	foreach(it,this->_players)
		standard_player_map[it->jid]=&(*it);

	std::stringstream ss(this->history_moves);
	std::string mv;
	for(int i=0 ; ss >> mv ; i=(i+1)%2) {
		this->chess.makeMove(mv);
		ss >> mv;
	}

	this->_title=this->_players[0].jid.node()+" x "+this->_players[1].jid.node();

	this->time_of_last_move=Util::Time();
	this->_resign=Chess::UNDEFINED;
	this->time_over=-1;
	this->turns_restart=0;
	this->_done=NOREASON;
	this->_done=(end_reason)realDone();

	delete adjourned_game;
}

const std::vector<XMPP::Jid>& GameChess::players() const {
    return this->_simple_players;
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
			if( ( chess.turn()==(colormap.find(it->jid)->second) ) and (this->turns_restart >= 2) and (this->_done==NOREASON or this->_done==TIMEOVER))
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
	return (generateStateTag(chess.getChessState(),current_time));
}

XML::Tag* GameChess::history() const {
	XML::Tag* h=generateHistoryTag();
	return ChessHistoryProcess::generate(h);
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

AdjournedGame* GameChess::adjourn(const Util::Time& current_time) {
	if(this->_done!=NOREASON)
		throw game_over("cannot adjourn, game already over");

	PlayerList p;
	foreach(it,this->_players)
		p.push_back(it->jid);

	XML::Tag* hist=this->generateHistoryTag(current_time-time_of_last_move);
	foreach(it,hist->tags())
		if(it->name()=="player")
			it->attributes().erase("score");

	return new ChessAdjournedGame(hist,p,this->category());
}

bool GameChess::done(const Util::Time& current_time) {
	if(this->_done==NOREASON) {
		if(this->turns_restart >= 2)
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
	if(this->_done!=NOREASON)
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
		prl[colormap.find(it->jid)->second==Chess::WHITE?0:1]=PlayerResult(it->jid,(it->color==White?"white":"black"),(""));
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

XML::Tag* GameChess::move(const Player& player, const std::string& movement, const Util::Time& time_stamp) {
	if(this->_done!=0)
		throw game_over("The game is already over");

	if(this->turns_restart >= 2)
		if((this->standard_player_map[player]->time)-time_stamp+time_of_last_move < Util::Time()) {
			this->_done=TIMEOVER;
			throw time_over::time_over(std::string("Time of ")+std::string(this->standard_player_map[player]->color==White?"white":"black")+std::string(" is over"));
		}

	if(colormap[player]!=chess.turn())
		throw wrong_turn(std::string("It's not ")+std::string(colormap[player]==Chess::WHITE?"white":"black")+std::string("'s turn"));

	if(chess.verifyAndMakeMove(movement)==false)
		throw invalid_move("Invalid Move");

	if(this->turns_restart >= 2)
		this->standard_player_map[player]->time-=time_stamp-time_of_last_move-this->standard_player_map[player]->inc;

	time_of_last_move=time_stamp;

	this->turns_restart++;

	this->_done=(end_reason)realDone();

	int last_time=int(this->standard_player_map[player]->time.getSeconds()+0.001);


	std::string realmove=movement;
	if(chess.hasThePawnPromoted() and movement.size()<5)
		realmove+="q";

	history_moves+=realmove+" "+Util::to_string(last_time)+" ";

	XML::TagGenerator move_tag;
	move_tag.openTag("move");
	move_tag.addAttribute("long",realmove);
	return move_tag.getTag();
}

XML::Tag* GameChess::generateHistoryTag(Util::Time time_passed) const {
	XML::TagGenerator gen;

	gen.openTag("history");
	gen.addAttribute("category",this->category());
	{
		gen.openTag("moves");
		{
			gen.addAttribute("movetext",this->history_moves.substr(0, this->history_moves.size()-1));
			gen.closeTag();
		}
		PlayerResultList prl=this->donePlayerResultList();
		foreach(it,prl) {
			gen.openTag("player");

			gen.addAttribute("jid",it->jid.full());

			gen.addAttribute("color",it->role);

			gen.addAttribute("score",it->score);

			gen.addAttribute("time",Util::to_string(this->initial_time));

			//TODO
			//confirm if time_left is only for adjourned games
			if(this->_done==NOREASON) {
				if( (this->chess.turn() == colormap.find(it->jid)->second) and (this->turns_restart>=2))
					gen.addAttribute("time_left",Util::to_string<int>(int((this->_players[it->role=="white"?0:1].time-time_passed).getSeconds()+0.001)));
				else
					gen.addAttribute("time_left",Util::to_string<int>(int(this->_players[it->role=="white"?0:1].time.getSeconds()+0.001)));
			}

			gen.addAttribute("inc",Util::to_string<int>(int(this->_players[0].inc.getSeconds()+0.001)));

			gen.closeTag();
		}
		gen.closeTag();
	}
	return gen.getTag();
}

//--------------------------------------
//CHESS GAME RESULT Stuff
//--------------------------------------

ChessGameResult::ChessGameResult(const std::string &endreason,const PlayerResultList &prl, const std::string& __category, XML::Tag* hist) : _history(hist) {
	this->_end_reason=endreason;
	this->player_result_list=prl;
	this->_category=__category;
}

ChessGameResult::~ChessGameResult() {
	delete this->_history;
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
	return new XML::Tag(*this->_history);
}

void ChessGameResult::updateRating(std::map<Player, Rating> &ratings) const {
	foreach(it,ratings) {
		if(it->second.countGames() == 0) {
			it->second.rating()=1500;
			it->second.volatility()=350.0;
		}
		else {
			it->second.volatility()=GlickoSystem::InitRD(it->second);
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

//--------------------------------------
//CHESS ADJOURNED GAME Stuff
//--------------------------------------

ChessAdjournedGame::ChessAdjournedGame(XML::Tag* __history, const PlayerList& __players, const std::string& category) : _history(__history), _players(__players), _category(category) {
}

ChessAdjournedGame::~ChessAdjournedGame() {
	delete this->_history;
}

XML::Tag* ChessAdjournedGame::history() const {
	return new XML::Tag(*this->_history);
}

const PlayerList& ChessAdjournedGame::players() const {
	return this->_players;
}

const std::string& ChessAdjournedGame::category() const {
	return this->_category;
}
