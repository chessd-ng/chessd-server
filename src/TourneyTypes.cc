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

#include "TourneyTypes.hh"
#include "Util/utils.hh"
#include "GameStandard.hh"

std::vector<Game*>* TourneyStandard::makeGames(const std::list<Pairing::Game>& games) const {
	std::vector<Game*>* g = new std::vector<Game*>;
	foreach(it,games) {
		StandardPlayerList players;
		players.push_back(StandardPlayer(XMPP::Jid(it->whiteName),this->initial_time,this->inc,White));
		players.push_back(StandardPlayer(XMPP::Jid(it->blackName),this->initial_time,this->inc,Black));
		g->push_back(new GameStandard(players));
	}
	return g;
}
