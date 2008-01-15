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
	return "standard";
}

Match* MatchRuleStandard::checkOffer(const XML::Tag& _match_offer, const TeamDatabase&) const {
	return new MatchStandard(getPlayersfromXML(_match_offer));
}

bool MatchRuleStandard::isTimeValid(const XML::Tag& _player) const {
	if(_player.hasAttribute("time")) {
		if(11u * Util::Minutes > Util::Time(_player.getAttribute("time"),Util::Minutes))
			return false;
	}
	return true;
}

/*
 * Coisas do MatchStandard
*/
MatchStandard::MatchStandard(const StandardPlayerList &players) :
	MatchChess(players),
	_category("standard")
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
