#ifndef TEAM_HH
#define TEAM_HH

#include <vector>
#include "XMPP/Jid.hh"

typedef XMPP::Jid Player;
typedef std::vector<Player> PlayerList; 
typedef std::vector<Player> Team;
typedef std::vector<Team> TeamList;

#endif
