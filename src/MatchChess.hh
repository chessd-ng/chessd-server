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

#include "MatchRule.hh"
#include "Match.hh"

class MatchRuleChess : public MatchRule {
	public:
		MatchRuleChess(const std::string& __category);

		virtual ~MatchRuleChess();

		virtual const std::string& getCategory() const;

		virtual Match* checkOffer(const XML::Tag& match_offer,
				const TeamDatabase& teams) const =0;

		std::vector<XML::Tag> getPlayersTag(const XML::Tag& match_offer) const;
	protected:
		StandardPlayerList getPlayersfromXML(const XML::Tag& _match_offer) const;

	private:
		std::string _category;

		void validateXML(const XML::Tag& _match_offer) const ;

		virtual bool isTimeValid(const XML::Tag& _player) const =0;
};

struct MatchChess : public Match {
	public:
		MatchChess(const std::vector<XML::Tag>& players, const std::string& __category);

		virtual ~MatchChess();
		
		virtual const PlayerList& players() const;

		virtual const std::string& category() const;

		virtual Game* createGame() const = 0;

		/*! \brief The offer notification */
		virtual XML::Tag* notification() const;
	protected:
		//this shouldn't be here
		std::vector<XML::Tag> _match_players;

		static StandardPlayerList getPlayersFromXML(const std::vector<XML::Tag>& players);

	private:
		std::string _category;
		//These two player list is necessary because StandardPlayer does not inherit from Players
		PlayerList _players;
};

struct MatchChessAdjourn : public MatchChess {
	public:
		MatchChessAdjourn(XML::Tag* _history) : MatchChess(this->getPlayersTag(*_history),_history->getAttribute("category")), history(_history) { }

		~MatchChessAdjourn() { delete this->history;}

	protected:
		XML::Tag* history;

	private:
		static std::vector<XML::Tag> getPlayersTag(const XML::Tag& history_adjourn);

};

#endif
