#ifndef MATCHMANAGER_HH
#define MATCHMANAGER_HH

#include <map>
#include <set>
#include "CoreInterface.hh"
#include "XMPP/Component.hh"
#include "XMPP/Node.hh"
#include "XMPP/Disco.hh"
#include "XMPP/Roster"
#include "Threads/Dispatch.hh"
#include "ComponentListener.hh"
#include "Util/Timer.hh"

#include "Match.hh"
#include "MatchRule.hh"

#include "Util/Sdb.hh"

class MatchManager : public Threads::Dispatcher {
	public:
		/*! \brief Constructor
		 *
		 * \param core_interface is the interface to the core
		 */
		MatchManager(CoreInterface* core_interface);

		/*! \brief Destructor
		 *
		 * Closes server connection if available
		 */
		~MatchManager();

		/*! \brief Connect to the server.
		 *
		 * \param host is the server address
		 * \param port is the port to be used
		 * \param password is the server's password
		 * \return Returns true on success, false otherwise.
		 */
		bool connect(const std::string& host, int port, const std::string& password);

		/*! \brief Insert a match rule
		 *
		 * The rule's ownership is passed to this class.
		 * \param rule is the MatchRule to be inserted.
		 */
		void insertMatchRule(MatchRule* rule);


	private:

		/* several handlers for the incoming events */

		/*! \brief handle an incoming match offer */
		void handleMatchOffer(Stanza* stanza);
		/*! \brief handle an incoming match acceptance */
		void handleMatchAccept(Stanza* stanza);
		/*! \brief handle an incoming match declinance */
		void handleMatchDecline(Stanza* stanza);

		/*! \brief Interface to the core */
		CoreInterface* core_interface;

		/*! \brief A XMPP component */
		XMPP::Component component;

		/*! \brief A component listener */
		ComponentListener listener;

		/*! \brief A XMPP node*/
		XMPP::Node node;

		/*! \brief A XMPP disco module */
		XMPP::Disco disco;

		typedef std::vector<PlayerID> Team;
		typedef SimpleDatabase<Team> TeamDB;

		/*! \brief Team database */
		TeamDB teams;

		typedef std::map<std::stream, MatchRule*>  RuleMap;
		/*! \brief Regitered rules */
		RuleMap rules;

		typedef SimpleDatabase<Match*> MatchDB;
		/*! \brief Pending offers */
		MatchDB matchs;
};

#endif
