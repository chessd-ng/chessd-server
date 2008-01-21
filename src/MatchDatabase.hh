#ifndef MATCHDATABASE_HH
#define MATCHDATABASE_HH

#include "Match.hh"
#include "Util/IDSet.hh"
#include "XMPP/Jid.hh"

#include <map>
#include <set>

#include <boost/ptr_container/ptr_map.hpp>

class MatchDatabase {
	public:
		MatchDatabase();
		~MatchDatabase();

		/*! \brief Insert a match to the database
		 *
		 * \param match is the match to be inserted
		 */
		int insertMatch(Match* match);

		/*! \brief Inform that a player accepted a match */
		void acceptMatch(int match_id, const XMPP::Jid& player);

		/*! \brief Ask whether this player blongs to the match.  */
		bool hasPlayer(int match_id, const XMPP::Jid& player) const;

		/*! \brief Get all the matchs which the player is in.
		 */
		const std::set<int>& getPlayerMatchs(const XMPP::Jid& player);

		/*! \brief Removes the match and return the Match instance
		 */
		Match* closeMatch(int match_id);

        /*! \brief Get a match by its id. */
		const Match& getMatch(int match_id) const;

        /*! \brief Get the ids of all active matchs */
        std::vector<int> getActiveMatchs() const;

        /*! \brief Ask whether everyone in the has accepted */
        bool isDone(int match_id) const;
		
	private:

		struct MatchInfo {
			std::auto_ptr<Match> match;
			std::map<XMPP::Jid, bool> accepted_players;
			int pending_count;
			MatchInfo(Match* match);
		};

        MatchInfo& findMatchInfo(int match_id);
        const MatchInfo& findMatchInfo(int match_id) const;

		Util::IDSet match_ids;
		boost::ptr_map<int, MatchInfo> matchs;
		std::map<XMPP::Jid, std::set<int> > player_matchs;
};

#endif
