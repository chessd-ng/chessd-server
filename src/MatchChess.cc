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

#include "MatchChess.hh"
#include "GameChess.hh"
#include "Util/utils.hh"
#include "GameException.hh"

/*
 * MatchChess stuff
*/

MatchChess::MatchChess(const std::vector<XML::Tag>& players, const std::string& __category) : 
	_match_players(players) ,
	_category(__category)
{
	foreach(it,players)
		this->_players.push_back(Player(it->getAttribute("jid")));
}

MatchChess::~MatchChess(){
}

const PlayerList& MatchChess::players() const {
	return this->_players;
}

const std::string& MatchChess::category() const {
	return this->_category;
}

Game* MatchChess::createGame() const {
	return new GameChess(this->getPlayersFromXML(this->_match_players),this->_category);
}

XML::Tag* MatchChess::notification() const {
	XML::TagGenerator t;
	t.openTag("match");
	t.addAttribute("category",this->category());
	foreach(it,this->_match_players)
		t.addChild(new XML::Tag(*it));
	t.closeTag();
	return t.getTag();
}

StandardPlayerList MatchChess::getPlayersFromXML(const std::vector<XML::Tag>& xml_players) {
	StandardPlayerList players;
	foreach(c_it,xml_players) {
		XMPP::Jid aux(c_it->getAttribute("jid"));
		Util::Time time(c_it->getAttribute("time"),Util::Seconds),inc;
		if(c_it->hasAttribute("inc"))
			inc=Util::Time(c_it->getAttribute("inc"),Util::Seconds);
		StandardPlayerColor c(c_it->getAttribute("color")=="white"?White:Black);
		players.push_back(StandardPlayer(aux,time,inc,c));
	}
	return players;
}

//MatchChessAdjourn stuff

std::vector<XML::Tag> MatchChessAdjourn::getPlayersTag(const XML::Tag& history_adjourn) {
	std::vector<XML::Tag> players;
	foreach(it,history_adjourn.tags())
		if(it->name()=="player")
			players.push_back(XML::Tag(*it));
	return players;
}

Game* MatchChessAdjourn::createGame() const {
	return new GameChess(this->history);
}

XML::Tag* MatchChessAdjourn::notification() const {
	XML::TagGenerator t;
	t.openTag("match");
	t.addAttribute("category",this->category());
	t.addAttribute("type","adjourned");
	foreach(it,this->_match_players)
		t.addChild(new XML::Tag(*it));
	t.closeTag();
	return t.getTag();
}

