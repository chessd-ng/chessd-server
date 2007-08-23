#include "MatchStandard.hh"

#include <memory>

MatchRuleStandard::MatchRuleStandard() {
}

MatchRuleStandard::~MatchRuleStandard() {
}

std::string MatchRuleStandard::getCategory() const {
	return "standard";
}

Match* MatchRuleStandard::checkOffer(MatchOffer* _match_offer, const Util::SimpleDatabase<Team>&) const {
	std::auto_ptr<MatchOffer> match_offer(_match_offer);
	if(match_offer->entities.size() != 2)
		throw "Wrong number of players";
	if(match_offer->entities[0].time != match_offer->entities[1].time or
			match_offer->entities[0].inc != match_offer->entities[1].inc)
		throw "Different times not allowed in standard game";
	if(match_offer->entities[0].time < 10*60 * Util::Seconds)
		throw "Time smaller than 10 minutes is not allowed in standard game";
	
	return new MatchStandard(std::make_pair(match_offer->entities[0], match_offer->entities[1]));
}

MatchStandard::MatchStandard(std::pair<MatchPlayer,MatchPlayer> players) {
	this->teams.push_back(MatchTeam(1,players.first));
	this->teams.push_back(MatchTeam(1,players.second));
}

MatchStandard::~MatchStandard() {
}

const MatchTeams& MatchStandard::getTeams() const {
	return this->teams;
}

std::string MatchStandard::getCategory() const {
	return "standard";
}
