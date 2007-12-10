#include "MatchStandard.hh"
#include "GameStandard.hh"
#include "Util/utils.hh"

#include <memory>

/*
 * Coisas do MatchRuleStandard
*/
MatchRuleStandard::MatchRuleStandard() {
}

MatchRuleStandard::~MatchRuleStandard() {
}

std::string MatchRuleStandard::getCategory() const {
	return "Standard";
}

Match* MatchRuleStandard::checkOffer(const XML::Tag& _match_offer, const TeamDatabase&) const {
	return new MatchStandard(getPlayersfromXML(_match_offer));
}

/*
 * Coisas do MatchStandard
*/
MatchStandard::MatchStandard(const StandardPlayerList &players) :
	MatchChess(players),
	_category("Standard")
{
}

MatchStandard::~MatchStandard() {
}

const std::string& MatchStandard::category() const {
	return _category;
}

Game* MatchStandard::createGame() const {
	return new GameStandard(this->_match_players);
}
