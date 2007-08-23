#include "MatchDatabase.hh"
#include "Util/utils.hh"

using namespace std;

MatchDatabase::MatchDatabase() {
}

MatchDatabase::~MatchDatabase() {
}

int MatchDatabase::insertMatch(Match* match) {
	int id = this->match_ids.acquireID();
	this->matchs.insert(make_pair(id, new MatchInfo(match)));
	foreach(team, match->getTeams()) {
		foreach(player, *team) {
			this->player_matchs[player->jid].insert(id);
		}
	}
	return id;
}

MatchDatabase::MatchInfo::MatchInfo(Match* match) : match(match), pending_count(0) {
	foreach(team, match->getTeams()) {
		foreach(player, *team) {
			this->accepted_players[player->jid]=0;
			this->pending_count ++;
		}
	}
}

bool MatchDatabase::acceptMatch(int match, const XMPP::Jid& player) {
	map<int, MatchInfo*>::iterator it = this->matchs.find(match);
	if(it == this->matchs.end())
		throw "Invalid match id";
	map<XMPP::Jid, bool>::iterator it2 = it->second->accepted_players.find(player);
	if(it2 == it->second->accepted_players.end())
		throw "Invalid match id";
	if(not it2->second)
		it->second->pending_count--;
	it2->second = true;
	return it->second->pending_count == 0;
}

bool MatchDatabase::hasPlayer(int match, const XMPP::Jid& player) {
	map<int, MatchInfo*>::iterator it = this->matchs.find(match);
	if(it == this->matchs.end())
		return false;
	map<XMPP::Jid, bool>::iterator it2 = it->second->accepted_players.find(player);
	if(it2 == it->second->accepted_players.end())
		return false;
	return true;
}

const set<int>& MatchDatabase::getPlayerMatchs(const XMPP::Jid& player) {
	return this->player_matchs[player];
}

Match* MatchDatabase::closeMatch(int id) {
	map<int, MatchInfo*>::iterator it = this->matchs.find(id);
	MatchInfo* match_info = it->second;
	Match* match = match_info->match.release();
	foreach(team, match->getTeams()) {
		foreach(player, *team) {
			this->player_matchs[player->jid].erase(id);
		}
	}
	this->match_ids.releaseID(id);
	this->matchs.erase(it);
	delete match_info;
	return match;
}

const Match& MatchDatabase::getMatch(int id) const {
	return *this->matchs.find(id)->second->match;
}
