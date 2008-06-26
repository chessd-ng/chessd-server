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

#ifndef TEAM_HH
#define TEAM_HH

#include <vector>
#include "XMPP/Jid.hh"
#include "Util/Timer.hh"
enum PLAYER_COLOR {
	WHITE=0,
	BLACK=1,
	UNDEFINED=-1
};

enum GAME_RESULT {
	WIN,LOSE,DRAW,NORESULT
};

/*! \brief All the info necessary for a player*/
struct GamePlayer {
	/* \brief creates a Player for Standard Chess
	 *
	 * \param jid: Jabber id from the player
	 * time:  The time for the Match
	 * inc: lag stuff
	 * color: the collor from the player in acord to StandardPlayerColor, it must be "black" or "white"
	 */
	GamePlayer(const XMPP::Jid &jid, const Util::Time &time, const Util::Time &inc, const PLAYER_COLOR color) :
		jid(jid),
		time(time),
		inc(inc),
		color(color) { }
	XMPP::Jid jid;
	Util::Time time, inc;
	PLAYER_COLOR color;
	/* \brief a Player is recognized by it's jid, so the less comparable signal is just comparing jids,
	 * considering that a player has a unique jid.
	 */
	bool operator <(const GamePlayer& p) const {
		return jid<p.jid;
	}
};

struct GamePlayerResult {
	GamePlayerResult(const GamePlayer& _player, const GAME_RESULT _result) : player(_player), result(_result) { }

	GamePlayer player;
	GAME_RESULT result;
};
/*
typedef XMPP::Jid Player;
typedef std::vector<Player> PlayerList; 
typedef std::vector<Player> Team;
typedef std::vector<Team> TeamList;
typedef std::vector<StandardPlayer> StandardPlayerList; 
typedef std::vector<PlayerResult> PlayerResultList;
*/
#endif
