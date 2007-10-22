#ifndef MATCHSTANDARD_HH
#define MATCHSTANDARD_HH

#include "MatchRule.hh"

class MatchRuleStandard : public MatchRule {
	public:
		MatchRuleStandard();

		virtual ~MatchRuleStandard();

		virtual std::string getCategory() const;

		virtual Match* checkOffer(XML::Tag& match_offer,
				const TeamDatabase& teamdb) const;
};

enum StandardPlayerColor {
	White,
	Black
};

struct StandardMatchPlayer {
	StandardMatchPlayer(XMPP::Jid jid, Util::Time time, Util::Time inc, StandardPlayerColor color) :
		jid(jid),
		time(time),
		inc(inc),
		color(color) { }
	XMPP::Jid jid;
	Util::Time time, inc;
	StandardPlayerColor color;
};

struct MatchStandard : public Match {
	public:

		MatchStandard(std::pair<StandardMatchPlayer, StandardMatchPlayer> players);

		virtual ~MatchStandard();
		
		virtual const PlayerList& players() const;

		virtual std::string getCategory() const;

		virtual Game* createGame() const;

	private:
		std::pair<StandardMatchPlayer, StandardMatchPlayer> _players;
};

#endif
