#ifndef MATCHCHESS_HH
#define MATCHCHESS_HH

#include "MatchRule.hh"
#include "Match.hh"

class MatchRuleChess : public MatchRule {
	public:
		MatchRuleChess();

		virtual ~MatchRuleChess();

		virtual std::string getCategory() const = 0;

		virtual Match* checkOffer(const XML::Tag& match_offer,
				const TeamDatabase& teams) const =0;
	protected:
		StandardPlayerList getPlayersfromXML(const XML::Tag& _match_offer) const;

	private:
		virtual bool validateXML(const XML::Tag& _match_offer) const;

		virtual bool isTimeValid(const XML::Tag& _player) const =0;
};

struct MatchChess : public Match {
	public:

		MatchChess(const StandardPlayerList &players);

		virtual ~MatchChess();
		
		virtual const PlayerList& players() const;

		virtual const std::string& category() const = 0;

		virtual Game* createGame() const = 0;

		/*! \brief The offer notification */
		virtual XML::Tag* notification() const;
	protected:
		//this shouldn't be here
		StandardPlayerList _match_players;
	private:
		//These two player list is necessary because StandardPlayer does not inherit from Players
		PlayerList _players;
};

#endif
