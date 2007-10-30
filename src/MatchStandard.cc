#include "MatchStandard.hh"

#include <memory>

MatchRuleStandard::MatchRuleStandard() {
}

MatchRuleStandard::~MatchRuleStandard() {
}

std::string MatchRuleStandard::getCategory() const {
	return "standard";
}

Match* MatchRuleStandard::checkOffer(XML::Tag& _match_offer, const TeamDatabase&) const {
/*	std::auto_ptr<MatchOffer> match_offer(_match_offer);
	if(match_offer->entities.size() != 2)
		throw "Wrong number of players";
	if(match_offer->entities[0].time != match_offer->entities[1].time or
			match_offer->entities[0].inc != match_offer->entities[1].inc)
		throw "Different times not allowed in standard game";
	if(match_offer->entities[0].time < 10*60 * Util::Seconds)
		throw "Time smaller than 10 minutes is not allowed in standard game";
	
	return new MatchStandard(std::make_pair(match_offer->entities[0], match_offer->entities[1]));*/
	return 0;
}

MatchStandard::MatchStandard(std::pair<StandardMatchPlayer,StandardMatchPlayer> players) :
	_match_players(players),
	_category("standard")
{
	this->_players.push_back(players.first.jid);
	this->_players.push_back(players.second.jid);
}

MatchStandard::~MatchStandard() {
}

const PlayerList& MatchStandard::players() const {
	return this->_players;
}

const std::string& MatchStandard::category() const {
	return _category;
}

Game* MatchStandard::createGame() const {
	return 0;
}
