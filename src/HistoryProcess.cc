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

#include "HistoryProcess.hh"
#include "XML/TagGenerator.hh"
#include "XML/Exception.hh"
#include "Util/utils.hh"
#include <sstream>
XML::Tag* ChessHistoryProcess::generate(XML::Tag* history_tag) {
	Chess chess;
	XML::TagGenerator new_history;
	std::string category=history_tag->getAttribute("category");

	/*
	 * position 0 is white
	 * position 1 is black
	*/
	std::vector<XML::Tag*> players(2);
	std::vector<int> last_time(2);

	foreach(it,history_tag->tags()) {
		if(it->name()=="player") {
			int p=((it->getAttribute("color")==std::string("white"))?0:1);
			players[p]=new XML::Tag(*it);
			players[p]->attributes().erase("score");
			players[p]->attributes().erase("time");
			last_time[p]=Util::parse_string<int>(it->getAttribute("time"));
		}
	}

	new_history.openTag("game");
	new_history.addAttribute("category",category);
	{
		new_history.addChild(generateStateTag(chess.getState(),players,last_time,category));

		std::stringstream s(history_tag->findChild("moves").getAttribute("movetext"));
		std::string move;
		for(int i=0;s >> move;i++) {
			if(!chess.verifyAndMakeMove(move))
				throw XML::xml_error("move of history's tag is invalid");

			s >> last_time[i%2];

			new_history.addChild(generateStateTag(chess.getState(),players,last_time,category));
		}
		new_history.closeTag();
	}
	foreach(it,players)
		delete *it;
	delete history_tag;
	return new_history.getTag();
}

XML::Tag* ChessHistoryProcess::generateStateTag(const ChessState& state, const std::vector<XML::Tag*>& players_without_time, const std::vector<int>& last_time, const std::string& category) {
	XML::TagGenerator new_state;
	new_state.openTag("state");
	new_state.addAttribute("category",category);
	{
		new_state.openTag("board");
		{
			new_state.addAttribute("state",state.boardFEN());
			new_state.addAttribute("turn",state.turn()==Chess::WHITE?"white":"black");
			new_state.addAttribute("castle",state.castle);
			if(state.enpassant.x!=-1)
				new_state.addAttribute("enpassant",state.enpassant.toStringNotation());
			else
				new_state.addAttribute("enpassant","-");
			new_state.addAttribute("halfmoves",Util::to_string(state.halfmoves));
			new_state.addAttribute("fullmoves",Util::to_string(state.fullmoves));
			new_state.closeTag();
		}
		for(unsigned int i=0;i<players_without_time.size();i++) {
			XML::Tag *aux= new XML::Tag(*players_without_time[i]);
			aux->setAttribute("time",Util::to_string(last_time[i]));
			new_state.addChild(aux);
		}
		new_state.closeTag();
	}
	return new_state.getTag();
}
