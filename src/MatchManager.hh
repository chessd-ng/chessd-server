#ifndef MATCHMANAGER_HH
#define MATCHMANAGER_HH

#include <map>
#include <set>
#include <memory>
#include <boost/ptr_container/ptr_map.hpp>
#include "CoreInterface.hh"
#include "XMPP/Component.hh"
#include "XMPP/RootNode.hh"
#include "XMPP/Disco.hh"
#include "XMPP/Roster.hh"
#include "Threads/Dispatcher.hh"
#include "Util/Timer.hh"

#include "Match.hh"
#include "MatchRule.hh"
#include "MatchProtocol.hh"
#include "MatchDatabase.hh"

#include "Query.hh"

#include "ComponentBase.hh"

class MatchManager : public ComponentBase {
	public:
		/*! \brief Constructor
		 *
		 * \param core_interface is the interface to the core.
		 * \param config is the configuration for this component.
		 */
		MatchManager(const XML::Tag& config, const XMPP::ErrorHandler& handleError);

		/*! \brief Destructor
		 *
		 * Closes server connection if available
		 */
		~MatchManager();

		/*! \brief Insert a match rule
		 *
		 * The rule's ownership is passed to this class.
		 * This must be called befre connect.
		 * \param rule is the MatchRule to be inserted.
		 */
		void insertMatchRule(MatchRule* rule);

	private:

		/* several handlers for the incoming events */

		/*! \brief handle an incoming match offer */
		void handleMatchOffer(Query* query);
		/*! \brief handle an incoming match acceptance */
		void handleMatchAccept(Query* query);
		/*! \brief handle an incoming match declinance */
		void handleMatchDecline(Query* query);
		/*! \brief handle an incoming match iq */
		void handleMatch(XMPP::Stanza* stanza);

		void notifyMatchOffer(int id, const XMPP::Jid& requester);

		void notifyMatchResult(Match* match, int id, bool accepted);

        void closeMatch(int id, bool accepted);

		void notifyUserStatus(const XMPP::Jid& jid, bool available);

		void _handleError(const std::string& error);

		void handleStanza(XMPP::Stanza* stanza);

        void onClose();

        void onError(const std::string& error);

		/*! \brief A XMPP roster */
		XMPP::Roster roster;

		/*! \brief Team database */
		TeamDatabase teams;

		typedef boost::ptr_map<std::string, MatchRule>  RuleMap;
		/*! \brief Registered rules */
		RuleMap rules;

		/*! \brief Pending offers */
		MatchDatabase match_db;

		Util::IDSet match_ids;

		XMPP::ErrorHandler handleError;
};

#endif
