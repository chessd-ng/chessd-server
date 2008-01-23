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

#ifndef MATCH_HH
#define MATCH_HH

#include <vector>
#include <set>

#include "XMPP/Jid.hh"
#include "Util/Timer.hh"
#include "Game.hh"
#include "Team.hh"

/* This match stuff is not flexible as it should be according
 * to the OO design. Each rule may require very different 
 * parameters, so it should not be fixed to time and color.
 * As it fit to all use cases, it will stay this way. But,
 * later it should be changed. A consequence is that the
 * communication should have something equally flexible,
 * such as the X forms in the xmpp. */

/*! \brief A plyer identification.
 *
 * This id is used locally by the match manager as an
 * alias for the player's jid. This is for the sake of
 * efficiency and convenience. Should this be at the
 * server core? */

/*! \brief Available colors.
 *
 * Possible color of the players. Again, what happen
 * if someone want to implment a game that has more
 * than two colors? */

/*enum PlayerColor {
	White,
	Black
};*/

/*! \brief A player regarding a match. */
/*struct MatchPlayer {
	MatchPlayer(XMPP::Jid jid, Util::Time time, Util::Time inc, PlayerColor color) :
		jid(jid),
		time(time),
		inc(inc),
		color(color) { }
	XMPP::Jid jid;
	Util::Time time, inc;
	PlayerColor color;
};

typedef std::vector<MatchPlayer> MatchTeam;

typedef std::vector<MatchTeam> MatchTeams;*/

/*! \brief An abstract match description. */
struct Match {

	public:

		/*! \brief Destructor */
		virtual ~Match() { }

		/*! \brief The players involved in the match */
		virtual const PlayerList& players() const = 0;

		virtual const std::string& category() const = 0;

		virtual Game* createGame() const = 0;

		/*! \brief The offer notification */
		virtual XML::Tag* notification() const = 0;
};

#endif
