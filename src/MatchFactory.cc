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

#include "MatchFactory.hh"
#include "MatchChess.hh"
#include "Util/utils.hh"
#include "GameException.hh"

Match* MatchFactory::create(const XML::Tag& match_offer,const TeamDatabase& teams) {
	validateXML(match_offer);
	return new MatchChess(getPlayersTag(match_offer),match_offer.getAttribute("category"));
}

bool MatchFactory::isTimeValid(const XML::Tag& _player,const std::string& category) {
	if(!_player.hasAttribute("time") and category!="untimed")
		return false;

	if(category=="standard") {
		if(11u * Util::Minutes <= Util::Time(_player.getAttribute("time"),Util::Seconds))
			return true;
	}
	else if(category=="blitz") {
		if((3u * Util::Minutes <= Util::Time(_player.getAttribute("time"),Util::Seconds)) and
				(10u * Util::Minutes >= Util::Time(_player.getAttribute("time"),Util::Seconds)))
			return true;
	}
	else if(category=="lightning") {
		if((1u * Util::Minutes <= Util::Time(_player.getAttribute("time"),Util::Seconds)) and
				(2u * Util::Minutes >= Util::Time(_player.getAttribute("time"),Util::Seconds)))
			return true;
	}
	else if(category=="untimed")
		return true;
	return false;
}

//FIXME
//does not work for untimed matches
void MatchFactory::validateXML(const XML::Tag& _match_offer) {
	if(_match_offer.name()!="match")
		throw bad_information("wrong matchrule xml name");

	if(_match_offer.hasAttribute("category")==false)
		throw bad_information("category missing");

	std::string category=_match_offer.getAttribute("category");

	if(_match_offer.getAttribute("category") != category)
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

			if(!c_it->hasAttribute("time") and category!="untimed")
				throw bad_information("xml does not have time for a player");

			if(category!="untimed") {
				if(time.first==true) {
					if(time.second!=c_it->getAttribute("time"))
						throw bad_information("time for players are diferent");
				}
				else
					time=make_pair(true,c_it->getAttribute("time"));
				if(isTimeValid(*c_it,category)==false)
					throw bad_information(std::string("invalid time for category ")+category);
			}
		}
	}
	if(count!=2)
		throw bad_information(std::string("Invalid number of players for category ")+category);
}

std::vector<XML::Tag> MatchFactory::getPlayersTag(const XML::Tag& match_offer) {
	validateXML(match_offer);
	std::vector<XML::Tag> ans;
	foreach(it,match_offer.tags())
		if(it->name()=="player")
			ans.push_back(XML::Tag(*it));
	return ans;
}
