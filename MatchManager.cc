#include <iostream>
#include <sigc++/sigc++.h>
#include "matchmanager.hh"
#include "task.hh"
#include "utils.hh"

using namespace std;
using namespace XML;

static void match_log_traffic(const std::string& data, bool incoming) {
	cout << "-----------------------------" << endl;
	cout << "MatchManager: " <<
		(incoming ? "Incoming!" : "Outcoming!") << endl;
	cout << "-----------" << endl;
	cout << data << endl;
	cout << "-----------------------------" << endl;
}

MatchManager::MatchManager(CoreInterface* interface, const JID& jid) :
	core_interface(core_interface),
	stream("jabber:component:accept"),
	stream_listener(this->createTunnel
			(sigc::mem_fun(&this->stream, Stream::handleTag))),
	component(sigc::mem_fun(&this->stream, Stream:send), secret),
	jabbernode(jid, sigc::mem_fun(&this->component, &Component::send)),
	disco(sigc::mem_fun(&this->component, &Component::send))
{
	this->stream.setTagHandler(&this->component, Component::handleTag);
	this->component.setLogHandler
		(sigc::ptr_fun(match_log_traffic));
	this->component.setConnectionHandler
		(sigc::mem_fun(this, &MatchManager::handleConnection));
	this->component.setRootStanzaHandler(sigc:mem_fun
			(&this->jabbernode, &JabberNode::handleStanza));
}

void MatchManager::connect(const std::string& host,
		int port, const std::string& password) {
	this->stream.connect(host, port, password);
	this->stream_listener.start();
	this->start();
}

/* this handles a stanza offering a match
 *
 * <iq type='set' from='jid requester' id=...>
 *   <query xmlns=.../match>
 *     <match category=category_name />
 *     <!-- list of players involved --!>
 *     <player jid= time= inc= color= />
 *     <player jid= time= inc= color= />
 *
 *     <team jid=nome@matchmanager.chessd time= inc= color= />
 *   </query>
 * </iq>
 *
 * <iq type='result' from='jid requester' id>
 *   <query xmlns=.../match>
 *     <match category=category_name match_id= />
 *     <!-- list of involved teams --!>
 *     <team>
 *       <player jid= time= inc= color= />
 *       <player jid= time= inc= color= />
 *     </team>
 *     <team>
 *       <player jid= time= inc= color= />
 *       <player jid= time= inc= color= />
 *     </team>
 *   </query>
 * </iq>
 */
void MatchManager::handleMatchOffer(Stanza* stanza) {
	ChildrenList::iterator it;
	Tag* query = stanza.children().front();
	Tag* tag_game = 0;
	vector<Tag*> player_tags;
	try {
		/* get the children that matters */
		foreach(child, query->children()) {
			Tag* tag = dynamic_cast<Tag*>(*child);
			if(tag==0) continue;
			if(tag->name()=="game") {
				if(tag_game!=0)
					throw("More than one game tag found");
				tag_game = tag;
			} else if(tag->name=="player") {
				player_tags.push_back(tag);
			}
		}

		foreach(tag, player_tags) {
			
		}


		JID player1_jid = JID(player1.getAttribute("jid"));
		JID player2_jid = JID(player2.getAttribute("jid"));
		/* the player who is making the challenge must
		 * be one of the players involved on the challenge */
		if(not stanza.from().parcialCompare(player1_jid)
				and not stanza.from().parcialCompare(player2_jid))
			throw(KeyError());
		
		const string& category = tag_game->getAttribute("category");
		RuleMap* rule = this->getMatchRule(category);
		
		if(rule->team_size != 1) {

			const set<JID>& team1 = this->getPlayerTeam(player1_jid);
			const set<JID>& team2 = this->getPlayerTeam(player1_jid);

			if(team1.size() != rule->team1 or team2.size)
	} catch (KeyError error) {
		stanza = Stanza::createErrorStanza(stanza,
				"modify",
				"bad-request");
		this->component.send(stanza);
	} catch (const char* msg) {
		stanza = Stanza::createErrorStanza(stanza,
				"modify",
				"bad-request",
				msg);
		this->component.send(stanza);
	}
}

void MatchManager::handleMatchAccept(Stanza* stanza) {
	
}

void MatchManager::handleMatchDecline(Stanza* stanza) {
	
}
