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

/*
 * Set initial variables.
*/
GameChessUntimed::GameChessUntimed(const StandardPlayerList& _players, const std::string &_category) {
	this->_category=_category;
	this->_players=_players;
	this->auto_flag=true;

	/*
	 * 0 is white
	 * 1 is black
	*/
	if(this->_players[0].color==Black)
		std::swap(this->_players[0],this->_players[1]);

	//fill players list
	foreach(it,this->_players) {
		standard_player_map[it->jid]=&(*it);
        _simple_players.push_back(it->jid);
    }

	this->setInitialVariables();
}

GameChessUntimed::GameChessUntimed(XML::Tag* adjourned_game) {
	this->_category=adjourned_game->getAttribute("category");

	//parse XML to get needed variables
	//this XML is specified in chessd protocol
	foreach(it,adjourned_game->tags()) {
		if(it->name()=="player") {
			this->_players.push_back(StandardPlayer(XMPP::Jid(it->getAttribute("jid")),
						Util::Time(),
						Util::Time(),
						StandardPlayerColor(it->getAttribute("color")=="white"?White:Black)));
			this->colormap[this->_players.rbegin()->jid]=this->_players.rbegin()->color==White?Chess::WHITE:Chess::BLACK;
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
	if(colormap[_simple_players[0]]==Chess::BLACK)
		std::swap(this->_simple_players[0],this->_simple_players[1]);

	foreach(it,this->_players)
		standard_player_map[it->jid]=&(*it);

	//Start the game where it stopped
	//just tell libchess where the game is

	this->setInitialVariables();


	if(this->_category=="untimed") {
		this->interpretHistoryMoves(this->history_moves);
		delete adjourned_game;
	}
}

void GameChessUntimed::interpretHistoryMoves(const std::string& moves) {
	std::stringstream ss(moves);
	std::string mv;
	for(int i=0 ; ss >> mv ; i=(i+1)%2)
		this->chess.makeMove(mv);
}

void GameChessUntimed::setInitialVariables() {
	//nobody resigned
	this->_resign=Chess::UNDEFINED;

	//the game is not over, it's starting or restarting
	this->_done=NOREASON;

	//Assumes that this->_players is already set
	//initiates colormap
	this->colormap[this->_players[0].jid]=this->_players[0].color==White?Chess::WHITE:Chess::BLACK;
	this->colormap[this->_players[1].jid]=this->_players[1].color==White?Chess::WHITE:Chess::BLACK;

	this->_title=this->_players[0].jid.node()+" x "+this->_players[1].jid.node();
}

const std::vector<XMPP::Jid>& GameChessUntimed::players() const {
    return this->_simple_players;
}

XML::Tag* GameChessUntimed::state(const Util::Time& current_time) const {
	XML::TagGenerator t;
	t.openTag("state");
	t.addAttribute("category",this->category());

	ChessState est=chess.getChessState();
	//board tag
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
		//players tag
		t.openTag("player");
		{   
			t.addAttribute("jid",it->jid.full());

			t.addAttribute("color",it->color==White?"white":"black");

			t.closeTag();
		}
	}
	t.closeTag();
	return t.getTag();
}

XML::Tag* GameChessUntimed::history() const {
	XML::Tag* h=generateHistoryTag();
	return ChessHistoryProcess::generate(h);
}

const std::string& GameChessUntimed::category() const {
	return this->_category;
}

const std::string& GameChessUntimed::title() const {
	return this->_title;
}

void GameChessUntimed::resign(const Player& player) {
	this->_resign=colormap[player]; //set the player who resigned
	this->_done=RESIGNED; //the game is over and the reason is one of the players resigned
}

void GameChessUntimed::call_flag(const Util::Time& current_time) {
	if(this->_done==NOREASON) {
		int aux;
		if((aux=chess.verifyDraw())!=0)
			//there are 3 reasons for drawing and chess.verifyDraw()
			//returns an int between 0 and 3 and the draw reasons begin
			//at value 5, that's whi +4 
			this->_done=(end_reason)(aux+4);
	}
}

void GameChessUntimed::draw() {
	//the players agreed on a draw
	this->_done=DRAWAGREED;
}

AdjournedGame* GameChessUntimed::adjourn(const Util::Time& current_time) {
	//you can't adjourn a game if it is over
	if(this->_done!=NOREASON)
		throw game_over("cannot adjourn, game already over");

	//the history of adjourned games only diffes of the history of the game
	//only by the score attribute, so it reuses code an delete the score attribute
	XML::Tag* hist=this->generateHistoryTag(current_time-time_of_last_move);
	foreach(it,hist->tags())
		if(it->name()=="player")
			it->attributes().erase("score");

	return new ChessAdjournedGame(hist,this->_simple_players,this->category());
}

bool GameChessUntimed::done(const Util::Time& current_time) {
	return this->_done!=0;
}

GameResult* GameChessUntimed::result() const {
	return new ChessGameResult(this->doneEndReason(),this->donePlayerResultList(),this->category(),this->generateHistoryTag());
}

int GameChessUntimed::realDone() {
	if(this->_done!=NOREASON)
		return this->_done;
	else if(chess.verifyCheckMate())
		return CHECKMATE;
	else {
		if(auto_flag==true)
			GameChessUntimed::call_flag(Util::Time());
	}
	return this->_done;
}

std::string GameChessUntimed::doneEndReason() const {
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
			return std::string("Time of ")+std::string((this->whoTimedOver()==int(White))?"white":"black")+std::string(" has ended");
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
		case DRAWTIMEOVER:
			return "Time of both players have ended";
			break;
		default:
			//this shouldn't happen because this function
			//is only supposed to be called when the game is over
			break;
	}
	return "";
}

PlayerResultList GameChessUntimed::donePlayerResultList() const {
	/* prl is indexed for
	 * 0 is white
	 * 1 is black*/
	PlayerResultList prl(2);
	foreach(it,_players)
		prl[(colormap.find(it->jid)->second==Chess::WHITE)?0:1]=PlayerResult(it->jid,(it->color==White?"white":"black"),(""));

	if(this->_done==3 or this->_done>=5) //if it is a draw
		prl[0].score=prl[1].score="1/2";
	else if (this->_done!=NOREASON) { //if the game ended
		bool aux=this->_resign==Chess::BLACK or (chess.winner()==Chess::WHITE) or (this->whoTimedOver()==1/*black*/);
		prl[0].score=(aux==true)?"1":"0";
		prl[1].score=(aux==true)?"0":"1";
	}
	return prl;
}

XML::Tag* GameChessUntimed::move(const Player& player, const std::string& movement, const Util::Time& time_stamp) {
	//can't make a move if the game is already over
	if(this->_done!=0)
		throw game_over("The game is already over");

	//can't move if it's not the player's turn
	if(colormap[player]!=chess.turn())
		throw wrong_turn(std::string("It's not ")+std::string(colormap[player]==Chess::WHITE?"white":"black")+std::string("'s turn"));

	//make the move
	if(chess.verifyAndMakeMove(movement)==false)
		throw invalid_move("Invalid Move"); //if the move isn't succesfull

	//set the _done variable here for optimization
	this->_done=(end_reason)realDone();

	std::string realmove=movement;
	//if there was a pawn promotion and it's not explicit
	if(chess.hasThePawnPromoted() and movement.size()<5)
		realmove+="q";

	//update history_moves
	history_moves+=realmove+" ";

	//generate move tag
	XML::TagGenerator move_tag;
	move_tag.openTag("move");
	move_tag.addAttribute("long",realmove);
	move_tag.addAttribute("short",this->chess.PGNOfLastMove());
	return move_tag.getTag();
}

XML::Tag* GameChessUntimed::generateHistoryTag(Util::Time time_passed) const {
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

			gen.closeTag();
		}
		gen.closeTag();
	}
	return gen.getTag();
}

/*
 * Game Chess Stuff
*/

GameChess::GameChess(const StandardPlayerList& _players, const std::string &category) : GameChessUntimed(_players,category) {
	this->initial_time=int(this->_players[0].time.getSeconds()+0.001);

	//time is not over for anyone
	this->time_over=-1;

	this->turns_restart=0; //0 turns since the restart of the game

	//no moves were made, so time is 0
	this->time_of_last_move=Util::Time();

}

GameChess::GameChess(XML::Tag* adjourned_game) : GameChessUntimed(adjourned_game) {
	//parse XML to get needed variables
	//this XML is specified in chessd protocol
	foreach(it,adjourned_game->tags()) {
		if(it->name()=="player") {
			this->_players[it->getAttribute("color")=="white"?White:Black].time=Util::Time(it->getAttribute("time_left"),Util::Seconds),
			this->_players[it->getAttribute("color")=="white"?White:Black].inc=Util::Time(it->getAttribute("inc"),Util::Seconds);
			this->initial_time=Util::parse_string<int>(it->getAttribute("time"));
		}
	}
	//time is not over for anyone
	this->time_over=-1;

	this->turns_restart=0; //0 turns since the restart of the game

	//no moves were made, so time is 0
	this->time_of_last_move=Util::Time();

	//interpret moves
	this->interpretHistoryMoves(this->history_moves);

	delete adjourned_game;
}

XML::Tag* GameChess::state(const Util::Time& current_time) const {
	XML::Tag* ans=GameChessUntimed::state(current_time);

	foreach(it,ans->tags()) {
		if(it->name()=="player") {
			Util::Time aux=this->_players[colormap.find(Player(it->getAttribute("jid")))->second].time;
			if( ( chess.turn()==(colormap.find(Player(it->getAttribute("jid")))->second) ) and (this->turns_restart >= 2) and (this->_done==NOREASON or this->_done==TIMEOVER))
				aux-=current_time-time_of_last_move;
			
			//XXX be careful with double from getSeconds
			if(this->auto_flag == true)
				it->attributes()["time"]= Util::to_string((int)(std::max(aux.getSeconds()+0.001,0.0)));
			else
				it->attributes()["time"]= Util::to_string((int)(aux.getSeconds()+0.001));
		}
	}

	return ans;
}

void GameChess::call_flag(const Util::Time& current_time) {
	this->checkTimeOver(current_time);
	GameChessUntimed::call_flag(current_time);
}

bool GameChess::done(const Util::Time& current_time) {
	if(this->_done==NOREASON)
		this->checkTimeOver(current_time);
	return GameChessUntimed::done(current_time);
}

XML::Tag* GameChess::move(const Player& player, const std::string& movement, const Util::Time& time_stamp) {
	//check if time is over
	if(this->auto_flag==true)
		if(this->checkTimeOver(time_stamp))
			throw time_over::time_over(std::string("Time of ")+std::string(this->chess.turn()==White?"white":"black")+std::string(" is over"));

	XML::Tag* ans=GameChessUntimed::move(player,movement,time_stamp);

	//count time
	if(this->turns_restart >= 2) 
		this->standard_player_map[player]->time-=time_stamp-time_of_last_move-this->standard_player_map[player]->inc;

	//update time_of_last_move
	time_of_last_move=time_stamp;

	//update turns since restart
	this->turns_restart++;

	//update history_moves
	int last_time=int(this->standard_player_map[player]->time.getSeconds()+0.001);
	history_moves+=Util::to_string(last_time)+" ";

	return ans;
}

bool GameChess::checkTimeOver(const Util::Time& current_time) {
	int conta=0;
	if(this->turns_restart >= 2) {
		for(int i=0;i<2;i++) {
			if(this->_players[i].time+((this->chess.turn()==i)?(this->time_of_last_move-current_time):Util::Time()) <= Util::Time()) {
				this->time_over=i;
				this->_done=TIMEOVER;
				conta++;
			}
		}
	}
	if(conta==2)
		this->_done=DRAWTIMEOVER;
	return conta>0;
}

XML::Tag* GameChess::generateHistoryTag(Util::Time time_passed) const {
	XML::Tag* ans=GameChessUntimed::generateHistoryTag(time_passed);
	foreach( it , ans->tags() ) {
		if(it->name()=="player") {
			it->attributes()["time"]=Util::to_string(this->initial_time);

			if(this->_done==NOREASON) {
				if( (this->chess.turn() == colormap.find(Player(it->getAttribute("jid")))->second) and (this->turns_restart>=2))
					it->attributes()["time_left"]=Util::to_string<int>(int((this->_players[(it->getAttribute("color")=="white")?0:1].time-time_passed).getSeconds()+0.001));
				else
					it->attributes()["time_left"]=Util::to_string<int>(int(this->_players[(it->getAttribute("color")=="white")?0:1].time.getSeconds()+0.001));
			}

			it->attributes()["inc"]=Util::to_string<int>(int(this->_players[0].inc.getSeconds()+0.001));

		}
	}
	return ans;
}

void GameChess::interpretHistoryMoves(const std::string& moves) {
	std::stringstream ss(moves);
	std::string mv;
	for(int i=0 ; ss >> mv ; i=(i+1)%2) {
		this->chess.makeMove(mv);
		ss >> mv;
	}
}

/*--------------------------------------
 *CHESS GAME RESULT Stuff
 *--------------------------------------*/

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
			//Step 1 of Glicko System
			it->second.volatility()=GlickoSystem::InitRD(it->second);
		}
	}
	std::vector<Player> playerlist;
	std::vector<Rating> playerrating;
	foreach(it,this->player_result_list) {
		playerlist.push_back(it->jid);
		playerrating.push_back(ratings[it->jid]);
	}

	//Glicko System formulas to calculate the new rating and new rating deviation
	int i=0,j=1;
	foreach(it,player_result_list) {
		double w;
		if(it->score == "1")
			playerrating[i].wins()++, w=1.0;
		else if(it->score == "1/2")
			playerrating[i].draws()++, w=0.5;
		else //it->score == "0"
			playerrating[i].losses()++, w=0.0;
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
	//update the rating
	for(i=0;i<(int)playerrating.size();i++)
		ratings[playerlist[i]]=playerrating[i];
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
