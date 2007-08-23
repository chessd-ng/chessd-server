#ifndef MATCHDATABASE_HH
#define MATCHDATABASE_HH

#include "Match.hh"
#include "Util/IDSet.hh"
#include "XMPP/Jid.hh"
#include <map>
#include <set>

class MatchDatabase {
	public:
		MatchDatabase();
		~MatchDatabase();

		/*! \brief Insert a match to the database
		 *
		 * \param match is the match to be inserted
		 */
		int insertMatch(Match* match);

		/*! \brief Inform that a player accepted a match
		 *
		 * It throws an exception if id is not found or
		 * if player is not in the match
		 */
		bool acceptMatch(int match, const XMPP::Jid& player);

		/*! \brief Inform that a player declined a match.
		 *
		 * It throws an exception if id is not found or
		 * if player is not in the match
		 */
		bool hasPlayer(int match, const XMPP::Jid& player);

		/*! \brief Get all the matchs which the player is in.
		 */
		const std::set<int>& getPlayerMatchs(const XMPP::Jid& player);

		/*! \brief Removes the match and return the Match instance
		 */
		Match* closeMatch(int id);

		const Match& getMatch(int id) const;
		
	private:

		Util::IDSet match_ids;
		struct MatchInfo {
			std::auto_ptr<Match> match;
			std::map<XMPP::Jid, bool> accepted_players;
			int pending_count;
			MatchInfo(Match* match);
		};
		std::map<int, MatchInfo*> matchs;
		std::map<XMPP::Jid, std::set<int> > player_matchs;
};

#endif
