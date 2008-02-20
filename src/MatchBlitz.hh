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

#ifndef MATCHBLITZ_HH
#define MATCHBLITZ_HH

#include "MatchChess.hh"

class MatchRuleBlitz : public MatchRuleChess {
	public:
		MatchRuleBlitz();

		virtual ~MatchRuleBlitz();

		virtual Match* checkOffer(const XML::Tag& match_offer,
				const TeamDatabase& teams) const ;

	private:
		virtual bool isTimeValid(const XML::Tag& _player) const ;
};

struct MatchBlitz : public MatchChess {
	public:

		MatchBlitz(const StandardPlayerList &players);

		virtual ~MatchBlitz();
		
		virtual Game* createGame() const;

	private:
		std::string _category;
};

#endif
