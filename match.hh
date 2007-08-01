#ifndef MATCH_HH
#define MATCH_HH

#include <vector>
#include <set>

/* This match stuff is not flexible as it should be according
 * to the OO design. Each rule may require very different 
 * parameters, so it should not be fixed to time and color.
 * As it fit to all use cases, it will stay this way. But,
 * later it should be changed. A consequence is that the
 * communication should have something equally flexible,
 * such as the X forms in the xmpp. */

/* This id is used locally by the match manager as an
 * alias for the player's jid. This is for the sake of
 * efficiency and convenience. Should this be at the
 * server core? */

typedef unsigned int PlayerID;

/* Possible color of the players. Again, what happen
 * if someone want to implmente a game that has more
 * than two colors? */

enum PlayerColor {
	White,
	Black
};

/* Here is defined the info about one entity of a match. */
struct MatchEntity {
	MatchEntity(PlayerID player, Time time, Time inc, PlayerColor color) :
		player(player) ,
		time(time) ,
		inc(inc) ,
		color(color) { }
	PlayerID player;
	Time time, inc;
	PlayerColor color;
};

/* For a match we have a collection of teams */
typedef std::vector<MatchEntity> MatchTeam;

typedef std::vector<MatchTeam> MatchTeams;

/* Here is an abstract match that holds information
 * about one single match */
struct Match {
	virtual ~Match() { }
	virtual MatchTeams getTeams() = 0 const;
};


#endif
