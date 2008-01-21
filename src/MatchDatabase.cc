#include "MatchDatabase.hh"
#include "Util/utils.hh"

using namespace std;

MatchDatabase::MatchDatabase() {
}

MatchDatabase::~MatchDatabase() {
}

int MatchDatabase::insertMatch(Match* match) {
	int id = this->match_ids.acquireID();
	this->matchs.insert(id, new MatchInfo(match));
	foreach(player, match->players()) {
		this->player_matchs[*player].insert(id);
	}
	return id;
}

MatchDatabase::MatchInfo::MatchInfo(Match* match) : match(match), pending_count(0) {
	foreach(player, match->players()) {
		this->accepted_players[*player] = 0;
		this->pending_count ++;
	}
}

void MatchDatabase::acceptMatch(int match_id, const XMPP::Jid& player) {
    MatchInfo& mi = this->findMatchInfo(match_id);
	map<XMPP::Jid, bool>::iterator it2 = mi.accepted_players.find(player);
	if(it2 == mi.accepted_players.end())
		throw "Invalid match id";
	if(not it2->second) {
		mi.pending_count--;
        it2->second = true;
    }
}

bool MatchDatabase::isDone(int match_id) const {
    return this->findMatchInfo(match_id).pending_count == 0;
}

bool MatchDatabase::hasPlayer(int match_id, const XMPP::Jid& player) const {
    const MatchInfo& mi = this->findMatchInfo(match_id);
	map<XMPP::Jid, bool>::const_iterator it2 = mi.accepted_players.find(player);
	if(it2 == mi.accepted_players.end())
		return false;
	return true;
}

const set<int>& MatchDatabase::getPlayerMatchs(const XMPP::Jid& player) {
	return this->player_matchs[player];
}

Match* MatchDatabase::closeMatch(int match_id) {
	boost::ptr_map<int, MatchInfo>::iterator it = this->matchs.find(match_id);
	MatchInfo& match_info = *it->second;
	Match* match = match_info.match.release();
    foreach(player, match->players()) {
		this->player_matchs[*player].erase(match_id);
    }
	this->match_ids.releaseID(match_id);
	this->matchs.erase(it);
	return match;
}

const Match& MatchDatabase::getMatch(int match_id) const {
	return *this->matchs.find(match_id)->second->match;
}

vector<int> MatchDatabase::getActiveMatchs() const {
    vector<int> ret;
    foreach(match, this->matchs) {
        if(match->second->pending_count > 0)
            ret.push_back(match->first);
    }
    return ret;
}

MatchDatabase::MatchInfo& MatchDatabase::findMatchInfo(int match_id) {
	boost::ptr_map<int, MatchInfo>::iterator it = this->matchs.find(match_id);
    if(it == this->matchs.end()) {
		throw "Invalid match id";
    }
    return *it->second;
}

const MatchDatabase::MatchInfo& MatchDatabase::findMatchInfo(int match_id) const {
	boost::ptr_map<int, MatchInfo>::const_iterator it = this->matchs.find(match_id);
    if(it == this->matchs.end()) {
		throw "Invalid match id";
    }
    return *it->second;
}
