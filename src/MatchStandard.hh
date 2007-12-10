#ifndef MATCHSTANDARD_HH
#define MATCHSTANDARD_HH

#include "MatchChess.hh"

class MatchRuleStandard : public MatchRuleChess {
	public:
		MatchRuleStandard();

		virtual ~MatchRuleStandard();

		virtual std::string getCategory() const;

		virtual Match* checkOffer(const XML::Tag& match_offer,
				const TeamDatabase& teams) const ;
};

struct MatchStandard : public MatchChess {
	public:

		MatchStandard(const StandardPlayerList &players);

		virtual ~MatchStandard();
		
		virtual const std::string& category() const;

		virtual Game* createGame() const;

	private:
		std::string _category;
};

#endif
