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
#include "Util/utils.hh"
#include "GameException.hh"

/*
 * MatchRuleChess Things
*/

MatchRuleChess::MatchRuleChess(const std::string& __category) : _category(__category) {
}

MatchRuleChess::~MatchRuleChess() {
}

const std::string& MatchRuleChess::getCategory() const {
	return this->_category;
}

//FIXME
//does not work for untimed matches
void MatchRuleChess::validateXML(const XML::Tag& _match_offer) const {
	if(_match_offer.name()!="match")
		throw bad_information("wrong matchrule xml name");

	if(_match_offer.hasAttribute("category")==false)
		throw bad_information("category missing");

	if(_match_offer.getAttribute("category") != this->getCategory())
		throw bad_information("category from xml incompatible with class game type");

	int count=0;
	std::set<std::string> colors;
	std::pair<bool,std::string> time;
	time.first=false;
	foreach(c_it,_match_offer.tags()) {
		if(c_it->name()=="player") {
			count++;
			if(!c_it->hasAttribute("jid"))
				throw bad_information("xml does not have jid for a player");

			if(!c_it->hasAttribute("color"))
				throw bad_information("xml does not have color for a player");

			if(c_it->getAttribute("color")!="white" and c_it->getAttribute("color")!="black")
				throw bad_information("wrong color for a player");

			if(colors.insert(c_it->getAttribute("color")).second==false)
				throw bad_information("Players of equal colors in the same match");

			if(!c_it->hasAttribute("time"))
				throw bad_information("xml does not have time for a player");

			if(time.first==true) {
				if(time.second!=c_it->getAttribute("time"))
					throw bad_information("time for players are diferent");
			}
			else
				time=make_pair(true,c_it->getAttribute("time"));
			if(isTimeValid(*c_it)==false)
				throw bad_information(std::string("invalid time for category ")+this->getCategory());
		}
	}
	if(count!=2)
		throw bad_information(std::string("Invalid number of players for category")+this->getCategory());
}

std::vector<XML::Tag> MatchRuleChess::getPlayersTag(const XML::Tag& match_offer) const {
	this->validateXML(match_offer);
	std::vector<XML::Tag> ans;
	foreach(it,match_offer.tags())
		if(it->name()=="player")
			ans.push_back(XML::Tag(*it));
	return ans;
}

StandardPlayerList MatchRuleChess::getPlayersfromXML(const XML::Tag& _match_offer) const {
	this->validateXML(_match_offer);
	StandardPlayerList players;
	foreach(c_it,_match_offer.children()) {
		if(typeid(*c_it)==typeid(XML::Tag)) {
			const XML::Tag& tag_c = static_cast<const XML::Tag&>(*c_it);
			XMPP::Jid aux(tag_c.getAttribute("jid"));
			Util::Time time(tag_c.getAttribute("time"),Util::Seconds),inc;
			if(tag_c.hasAttribute("inc"))
				inc=Util::Time(tag_c.getAttribute("inc"),Util::Seconds);
			StandardPlayerColor c(tag_c.getAttribute("color")=="white"?White:Black);
			players.push_back(StandardPlayer(aux,time,inc,c));
		}
	}
	return players;
}

/*
 * MatchChess things
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

std::vector<XML::Tag> MatchChessAdjourn::getPlayersTag(const XML::Tag& history_adjourn) {
	std::vector<XML::Tag> players;
	foreach(it,history_adjourn.tags())
		if(it->name()=="player")
			players.push_back(XML::Tag(*it));
	return players;
}
