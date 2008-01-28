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

#include <iostream>
#include <cmath>
#include "Util/utils.hh"
#include "GameStandard.hh"
#include "GlickoSystem.hh"
//TODO
//tempo
//testar XML

GameStandard::GameStandard(const StandardPlayerList& _players) :
	GameChess(_players,"standard")
{
}

GameResult* GameStandard::result() const {
	bool checkmate;
	std::string reason;
	TeamResultList trl;
	for(int i=0;i<(int)_teams.size();i++)
		trl.push_back(make_pair(_teams[i],UNDEFINED));

	if((checkmate=chess.verifyCheckMate()) or this->_resign!=Chess::UNDEFINED) {
		reason=(checkmate==true)?"Checkmate":"The other player resigned";
		if(this->_resign==Chess::BLACK or (chess.winner()==Chess::WHITE)) {
			trl[0].second=WINNER;
			trl[1].second=LOSER;
		}
		else {
			trl[1].second=WINNER;
			trl[0].second=LOSER;
		}
	}
	else if(this->_draw==true) {
		trl[0].second=trl[1].second=DRAWER;
		reason="The players agreed on a draw";
	}
	else if(chess.verifyDraw()==true) {
		trl[0].second=trl[1].second=DRAWER;
		reason="Draw";
	}

	return new ChessStandardGameResult(reason,trl,chess.getHistory());
}

ChessStandardGameResult::ChessStandardGameResult(const std::string &endreason,const TeamResultList &l,const std::vector<State> &s) :
	ChessGameResult(endreason,l,"standard",s)
{
}

void ChessStandardGameResult::updateRating(std::map<Player, Rating> &ratings) const {
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
