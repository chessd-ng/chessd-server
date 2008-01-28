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

#include "MatchChess.hh"
#include "Util/utils.hh"
#include "GameException.hh"

/*
 * MatchRuleChess Things
*/

MatchRuleChess::MatchRuleChess() {
}

MatchRuleChess::~MatchRuleChess() {
}

void MatchRuleChess::validateXML(const XML::Tag& _match_offer) const {
	if(_match_offer.name()=="match") {
		if(_match_offer.hasAttribute("category")) {
			if(_match_offer.getAttribute("category") == this->getCategory()) {
				int conta=0;
				foreach(c_it,_match_offer.tags()) {
					if(c_it->name()=="player") {
						conta++;
						if(!c_it->hasAttribute("jid"))
							throw bad_information("xml does not have jid for a player");
						if(!c_it->hasAttribute("color"))
							throw bad_information("xml does not have color for a player");
						if(!c_it->hasAttribute("time"))
							throw bad_information("xml does not have time for a player");
						if(isTimeValid(*c_it)==false)
							throw bad_information(std::string("invalid time for category ")+this->getCategory());
					}
				}
				if(conta==2)
					throw bad_information(std::string("Invalid number of players for category")+this->getCategory());
			}
			else
				throw bad_information("wrong category name");
		}
		else
			throw bad_information("category missing");
	}
	else
		throw bad_information("wrong matchrule xml name");
	throw bad_information("xml is wrong in someway");
}

StandardPlayerList MatchRuleChess::getPlayersfromXML(const XML::Tag& _match_offer) const {
	StandardPlayerList players;
	foreach(c_it,_match_offer.children()) {
		if(typeid(*c_it)==typeid(XML::Tag)) {
			const XML::Tag& tag_c = static_cast<const XML::Tag&>(*c_it);
			XMPP::Jid aux(tag_c.getAttribute("jid"));
			Util::Time time(tag_c.getAttribute("time"),Util::Seconds),inc;
			if(tag_c.hasAttribute("inc"))
				inc=Util::Time(tag_c.getAttribute("inc"),Util::Seconds);
			StandardPlayerColor c(tag_c.getAttribute("color")=="w"?White:Black);
			players.push_back(StandardPlayer(aux,time,inc,c));
		}
	}
	return players;
}

/*
 * MatchChess things
*/

MatchChess::MatchChess(const StandardPlayerList &players) : 
	_match_players(players)
{
	foreach(it,players)
		this->_players.push_back(Player(it->jid));
}

MatchChess::~MatchChess(){
}

const PlayerList& MatchChess::players() const {
	return this->_players;
}

XML::Tag* MatchChess::notification() const {
	XML::TagGenerator t;
	t.openTag("match");
	t.addAttribute("category",this->category());
	foreach(it,_match_players) {
		t.openTag("player");
		{
			t.addAttribute("jid",it->jid.full());

			char tmp[8];
			int tempo=(int)(it->time.getSeconds()+0.5);
			sprintf(tmp,"%d",tempo);
			t.addAttribute("time",std::string(tmp));

			tempo=(int)(it->inc.getSeconds()+0.5);
			sprintf(tmp,"%d",tempo);
			t.addAttribute("inc",std::string(tmp));

			t.addAttribute("color",it->color==White?"w":"b");
		}
		t.closeTag();
	}
	t.closeTag();
	return t.getTag();
}
