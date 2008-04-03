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

#ifndef MATCHCHESS_HH
#define MATCHCHESS_HH

#include "MatchFactory.hh"
#include "Match.hh"
#include "XML/Xml.hh"

struct MatchChess : public Match {
	public:
		MatchChess(const std::vector<XML::Tag>& players, const std::string& __category);

		virtual ~MatchChess();
		
		virtual const PlayerList& players() const;

		virtual const std::string& category() const;

		virtual Game* createGame() const ;

		/*! \brief The offer notification */
		virtual XML::Tag* notification() const;

	protected:
		/*this is protected just only for MatchChessAdjourn*/
		std::vector<XML::Tag> _match_players;

	private:
		std::string _category;

		PlayerList _players;

		static StandardPlayerList getPlayersFromXML(const std::vector<XML::Tag>& players);

};

struct MatchChessAdjourn : public MatchChess {
	public:
		MatchChessAdjourn(XML::Tag* _history) : MatchChess(this->getPlayersTag(*_history),_history->getAttribute("category")), history(_history) { }

		virtual Game* createGame() const;

		virtual XML::Tag* notification() const;
	private:
		static std::vector<XML::Tag> getPlayersTag(const XML::Tag& history_adjourn);

		XML::Tag* history;
};

#endif
