#ifndef MATCH_HH
#define MATCH_HH

#include <vector>
#include <set>

#include "XMPP/Jid.hh"
#include "Util/Timer.hh"

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

enum PlayerColor {
	White,
	Black
};

/*! \brief A player regarding a match. */
struct MatchPlayer {
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

typedef std::vector<MatchTeam> MatchTeams;

/*! \brief An abstract match description. */
struct Match {

	public:

		/*! \brief Destructor */
		virtual ~Match() { }

		/*! \brief The teams involved in the match */
		virtual const MatchTeams& getTeams() const = 0;

		virtual std::string getCategory() const = 0;
};

struct MatchOffer {
	std::string category;
	std::vector<MatchPlayer> entities;
};

#endif
