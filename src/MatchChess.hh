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
	protected:
		StandardPlayerList getPlayersfromXML(const XML::Tag& _match_offer) const;

	private:
		std::string _category;

		virtual void validateXML(const XML::Tag& _match_offer) const;

		virtual bool isTimeValid(const XML::Tag& _player) const =0;
};

struct MatchChess : public Match {
	public:
		MatchChess(const StandardPlayerList& players, const std::string& __category);

		virtual ~MatchChess();
		
		virtual const PlayerList& players() const;

		virtual const std::string& category() const ;

		virtual Game* createGame() const = 0;

		/*! \brief The offer notification */
		virtual XML::Tag* notification() const;
	protected:
		//this shouldn't be here
		StandardPlayerList _match_players;

	private:
		std::string _category;
		//These two player list is necessary because StandardPlayer does not inherit from Players
		PlayerList _players;
};

#endif
