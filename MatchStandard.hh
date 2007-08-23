#ifndef MATCHSTANDARD_HH
#define MATCHSTANDARD_HH

#include "MatchRule.hh"

class MatchRuleStandard : public MatchRule {
	public:
		MatchRuleStandard();

		virtual ~MatchRuleStandard();

		virtual std::string getCategory() const;

		virtual Match* checkOffer(MatchOffer* match_offer,
				const Util::SimpleDatabase<Team>& teamdb) const;
};

struct MatchStandard : public Match {
	public:

		MatchStandard(std::pair<MatchPlayer,MatchPlayer> players);

		virtual ~MatchStandard();
		
		virtual const MatchTeams& getTeams() const;

		virtual std::string getCategory() const;

	private:
		MatchTeams teams;
};

#endif
