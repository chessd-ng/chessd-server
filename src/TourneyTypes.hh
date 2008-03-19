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

#ifndef TOURNEYTYPES_HH
#define TOURNEYTYPES_HH

#include "ChessTourney.hh"

class TourneyStandard : public ChessTourney {
	public:
		TourneyStandard(const Util::Time& _initial_time, const Util::Time& _inc, int rounds) : ChessTourney(_initial_time,_inc, rounds) { }

	protected:
		virtual std::vector<Game*>* makeGames(const std::list<Pairing::Game>& games) const;

	private:
};
#endif
