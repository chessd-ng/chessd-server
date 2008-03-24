/*
 *   Copyright (c) 2007-2008 C3SL.
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

#include "ChessTourney.hh"
#include "TourneyException.hh"
#include <algorithm>

ChessTourney::ChessTourney(const Util::Time& _initial_time, const Util::Time& _inc, int __rounds) : initial_time(_initial_time), inc(_inc), _rounds(__rounds), _missing_rounds(__rounds), tourney('s') {
	tourney_started=false;
	missing_results=false;
}

const TourneyPlayerList& ChessTourney::players() const {
	return this->_players;
}

void ChessTourney::addPlayer(const TourneyPlayer& p) {
	this->tourney.AddPlayer(p.jid.full(),p.rating.rating());
	this->_players.push_back(p);
	this->player_map[p.jid]=this->_players.size()-1;
}

std::vector<Game*>* ChessTourney::match() {
	if(missing_results==false) {
		if(this->_missing_rounds==0)
			throw tourney_over("Can't make one more round, number of rounds already reached");
		this->_missing_rounds--;
		if(tourney_started)
			this->tourney.MakeAssignments();
		else {
			this->tourney.start();
			tourney_started=true;
		}

		missing_results=true;
		result_set.clear();

		return this->makeGames(this->tourney.getGames());
	}
	return 0;
}

void ChessTourney::addResult(const PlayerResultList& prl) {
	foreach(it,prl) {
		if(result_set.insert(it->jid).second==false)
			throw result_error("Cannot set result, already set");
		if(player_map.find(it->jid)==player_map.end())
			throw result_error("Player not found to set result");
	}
	result_set.insert(prl[0].jid);
	result_set.insert(prl[1].jid);
	missing_results=tourney.SetGameResult(prl[0].jid.full(),prl[1].jid.full(),(prl[0].score!="1/2"?(prl[0].score=="1"?1:0):2))!=2;
	this->_players[player_map[prl[0].jid]].score=tourney.GetPlayer(prl[0].jid.full())->score;
	this->_players[player_map[prl[1].jid]].score=tourney.GetPlayer(prl[1].jid.full())->score;
}
