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

#include "MatchBlitz.hh"
#include "GameBlitz.hh"
#include "Util/utils.hh"

#include <memory>

/*
 * Coisas do MatchRuleBlitz
*/
MatchRuleBlitz::MatchRuleBlitz() : MatchRuleChess("blitz") {
}

MatchRuleBlitz::~MatchRuleBlitz() {
}

Match* MatchRuleBlitz::checkOffer(const XML::Tag& _match_offer, const TeamDatabase&) const {
	return new MatchBlitz(getPlayersfromXML(_match_offer));
}

bool MatchRuleBlitz::isTimeValid(const XML::Tag& _player) const {
	if(_player.hasAttribute("time")) {
		if((3u * Util::Minutes <= Util::Time(_player.getAttribute("time"),Util::Minutes)) and
				(10u * Util::Minutes >= Util::Time(_player.getAttribute("time"),Util::Minutes)))
			return true;
	}
	return false;
}

/*
 * Coisas do MatchBlitz
*/
MatchBlitz::MatchBlitz(const StandardPlayerList &players) :
	MatchChess(players,"blitz")
{
}

MatchBlitz::~MatchBlitz() {
}

Game* MatchBlitz::createGame() const {
	return new GameBlitz(this->_match_players);
}
