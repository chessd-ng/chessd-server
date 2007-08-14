#ifndef MATCH_HH
#define MATCH_HH

#include <vector>
#include <set>

#include "XMPP/Jid.hh"
#include "Util/Time.hh"

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

typedef XMPP::Jid PlayerID;

/*! \brief Available colors.
 *
 * Possible color of the players. Again, what happen
 * if someone want to implment a game that has more
 * than two colors? */

enum PlayerColor {
	White,
	Black
};

/*! \brief An entity regarding a match. */
struct MatchEntity {
	MatchEntity(PlayerID player, Time time, Time inc, PlayerColor color) :
		player(player),
		time(time),
		inc(inc),
		color(color) { }
	PlayerID player;
	Time time, inc;
	PlayerColor color;
};

typedef std::vector<MatchEntity> MatchTeam;

typedef std::vector<MatchTeam> MatchTeams;

/*! \brief An abstract match description. */
struct Match {

	/*! \brief Destructor */
	virtual ~Match() { }

	/*! \brief The teams involved in the match */
	virtual const MatchTeams& getTeams() = 0 const;
};


#endif
