#ifndef MATCHMANAGER_HH
#define MATCHMANAGER_HH

#include <map>
#include <set>
#include <sigc++/sigc++.h>
#include "core_interface.hh"
#include "component.hh"
#include "jabbernode.hh"
#include "dispatch.hh"
#include "stream_listener.hh"
#include "disco.hh"
#include "timer.hh"

#include "match.hh"
#include "matchrule.hh"

#include "sdb.hh"

#include "identifier.hh"

class MatchManager : public Dispatcher {
	public:
		MatchManager(CoreInterface* core_interface, const JID& jid);

		~MatchManager();

		void connect(const std::string& host, const std::string& password, int port);

		void insertMatchRule(const MatchRule& rule);


	private:

		/* Keep a relation from JID and ID */
		typedef Identifier<JID> JidIdentifier;
		JidIdentifier jid_identifier;

		/* several handlers for the incoming events */

		/* handle the conenction status */
		void handleConnection(int status);

		/* handle an incoming match offer */
		void handleMatchOffer(Stanza* stanza);
		/* handle an incoming match acceptance */
		void handleMatchAccept(Stanza* stanza);
		/* handle an incoming match declinance */
		void handleMatchDecline(Stanza* stanza);

		/* all required structures for jabber */
		CoreInterface* core_interface;
		Stream stream;
		StreamListener listener;
		Component component;
		Jabbernode node;
		Disco disco;

		/* Team database */
		typedef std::vector<PlayerID> Team;
		typedef SimpleDatabase<Team> TeamDB;
		TeamDB teams;

		/* types and members used to store the match rules */
		typedef std::map<std::stream, MatchRule*>  RuleMap;
		RuleMap rules;

		/* search for a match rule */
		/* Returns NULL if category is not found */
		RuleMap* getMatchRule(const std::string& category);
		void insertMatchRule(MatchRule* rule, const std::string& category);

		/* types and members used to store current matchs */
		typedef SimpleDatabase<Match*> MatchDB;
		MatchDB matchs;

		/* keep track of monitored users */
		typedef std::set<PlayerID> IDSet;
		IDSet monitored_users;
};

#endif
