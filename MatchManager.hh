#ifndef MATCHMANAGER_HH
#define MATCHMANAGER_HH

#include <map>
#include <set>
#include "CoreInterface.hh"
#include "XMPP/Component.hh"
#include "XMPP/Node.hh"
#include "XMPP/Disco.hh"
#include "XMPP/Roster.hh"
#include "Threads/Dispatcher.hh"
#include "ComponentListener.hh"
#include "Util/Timer.hh"

#include "Match.hh"
#include "MatchRule.hh"

#include "Util/Sdb.hh"

class MatchManager {
	public:
		/*! \brief Constructor
		 *
		 * \param core_interface is the interface to the core.
		 * \param config is the configuration for this component.
		 */
		MatchManager(CoreInterface* core_interface, const XML::Tag* config);

		/*! \brief Destructor
		 *
		 * Closes server connection if available
		 */
		~MatchManager();

		/*! \brief Connect to the server.
		 *
		 * \return Returns true on success, false otherwise.
		 */
		bool connect();

		/*! \brief Insert a match rule
		 *
		 * The rule's ownership is passed to this class.
		 * This must be called befre connect.
		 * \param rule is the MatchRule to be inserted.
		 */
		void insertMatchRule(MatchRule* rule);

		/*! \brief Closes the conenction to the server */
		void close();


	private:

		/* several handlers for the incoming events */

		/*! \brief handle an incoming match offer */
		void handleMatchOffer(XMPP::Stanza* stanza);
		/*! \brief handle an incoming match acceptance */
		void handleMatchAccept(XMPP::Stanza* stanza);
		/*! \brief handle an incoming match declinance */
		void handleMatchDecline(XMPP::Stanza* stanza);

		/*! \brief We run in a separated thread as a dispatcher */
		Threads::Dispatcher dispatcher;

		/*! \brief Is it running? */
		bool running;

		/*! \brief Interface to the core */
		CoreInterface* core_interface;

		/*! \brief A XMPP component */
		XMPP::Component component;

		/*! \brief A component listener */
		ComponentListener listener;

		/*! \brief A XMPP node*/
		XMPP::Node root_node;

		typedef std::vector<PlayerID> Team;
		typedef Util::SimpleDatabase<Team> TeamDB;

		/*! \brief Team database */
		TeamDB teams;

		typedef std::map<std::string, MatchRule*>  RuleMap;
		/*! \brief Registered rules */
		RuleMap rules;

		typedef Util::SimpleDatabase<Match*> MatchDB;
		/*! \brief Pending offers */
		MatchDB matchs;

		/*! \brief The XMPP server address */
		std::string server_address;

		/*! \brief The server port */
		int server_port;

		/*! \brief The server password */
		std::string server_password;
};

#endif
