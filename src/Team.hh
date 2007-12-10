#ifndef TEAM_HH
#define TEAM_HH

#include <vector>
#include "XMPP/Jid.hh"
#include "Util/Timer.hh"
enum StandardPlayerColor {
	White,
	Black
};
/*! \brief All the info necessary for a player
*/
struct StandardPlayer {
	/* \brief creates a Player for Standard Chess
	 *
	 * \param jid: Jabber id from the player
	 * time:  The time for the Match
	 * inc: lag stuff
	 * color: the collor from the player in acord to StandardPlayerColor, it must be "black" or "white"
	 */
	StandardPlayer(const XMPP::Jid &jid, const Util::Time &time, const Util::Time &inc, const StandardPlayerColor& color) :
		jid(jid),
		time(time),
		inc(inc),
		color(color) { }
	XMPP::Jid jid;
	Util::Time time, inc;
	StandardPlayerColor color;
	/* \brief a Player is recognized by it's jid, so the less comparable signal is just comparing jids,
	 * considering that a player has a unique jid.
	 */
	bool operator <(const StandardPlayer& p) const {
		return jid<p.jid;
	}
};

typedef XMPP::Jid Player;
typedef std::vector<Player> PlayerList; 
typedef std::vector<Player> Team;
typedef std::vector<Team> TeamList;
typedef std::vector<StandardPlayer> StandardPlayerList; 

#endif
