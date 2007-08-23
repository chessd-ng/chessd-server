#include <memory>
#include <iostream>
#include "MatchManager.hh"
#include "Threads/Task.hh"
#include "Util/utils.hh"

#include "MatchStandard.hh"


using namespace std;
using namespace XML;
using namespace XMPP;

void match_log_traffic(const std::string& data, bool incoming) {
	cout << "-----------------------------" << endl;
	cout << "MatchManager: " <<
		(incoming ? "Incoming!" : "Outcoming!") << endl;
	cout << "-----------" << endl;
	cout << data << endl;
	cout << "-----------------------------" << endl;
}

MatchManager::MatchManager(CoreInterface* interface, const XML::Tag& config) :
	running(false),
	core_interface(interface),
	component(config.getAttribute("node_name")),
	listener(this->component),
	root_node(boost::bind(&Component::send, &this->component, _1), "Match Manager", "service", "game"),
	roster(boost::bind(&Component::send, &this->component, _1),
			boost::bind(&MatchManager::notifyUserStatus, this, _1, _2)),
	server_address(config.getAttribute("server_address")),
	server_port(Util::str2int(config.getAttribute("server_port"))),
	server_password(config.getAttribute("server_password")),
	protocol("protocol/")
{
	/* Set root_node to receive stanzas from the component */
	XMPP::StanzaHandler f = boost::bind(&Node::handleStanza, &this->root_node, _1);
	this->component.setRootStanzaHandler(this->dispatcher.createTunnel(f));

	/* Set the root_node to route presence stanzas to the roster */
	this->root_node.setPresenceHandler(boost::bind(&Roster::handlePresence, &this->roster, _1));

	/* Set features */
	this->root_node.disco().features().insert("presence");
	this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#match");

	/* Set match iqs */
	this->root_node.setIqHandler(boost::bind(&MatchManager::handleMatch, this, _1),
			"http://c3sl.ufpr.br/chessd#match");

	this->insertMatchRule(new MatchRuleStandard);
}

void MatchManager::insertMatchRule(MatchRule* rule) {
	this->rules.insert(make_pair(rule->getCategory(), rule));
}

MatchManager::~MatchManager() {
	if(this->running)
		this->close();
	/*foreach(match, this->matchs)
		delete *match;*/
	foreach(rule, this->rules)
		delete rule->second;
}

bool MatchManager::connect() {
	if(not this->component.connect(this->server_address, this->server_port, this->server_password)) {
		return false; // TODO throw an exception
	} else {
		this->running = true;
		this->dispatcher.start();
		this->listener.start();
		return true;
	}
}

void MatchManager::close() {
	this->listener.stop();
	this->dispatcher.stop();
	this->component.close();
	this->running = false;
}

void MatchManager::handleMatch(Stanza* stanza) {
	Tag& query = *stanza->children().front();
	string action = this->protocol.parseMatch(query);
	if(action == "offer") {
		this->handleMatchOffer(stanza);
	} else if(action == "accept") {
		this->handleMatchAccept(stanza);
	} else if(action == "decline") {
		this->handleMatchDecline(stanza);
	} else {
		this->component.send(Stanza::createErrorStanza(stanza, "cancel", "bad-request"));
	}
}

void MatchManager::handleMatchOffer(Stanza* _stanza) {
	auto_ptr<MatchOffer> offer;
	auto_ptr<Match> match;
	auto_ptr<Stanza> stanza(_stanza);
	Jid requester = stanza->from();
	try {
		/* parse message */
		offer = auto_ptr<MatchOffer>(this->protocol.parseMatchOffer(**stanza->children().begin()));

		/* check players availability and whether
		 * to sender is one of the entities */
		bool sanity = false;
		foreach(player, offer->entities) {
			if(not this->roster.isUserAvailable(player->jid))
				throw "User is unavailable";
			if(player->jid == requester) sanity = true;
			/* check if there are repeated players */
			int count = 0;
			foreach(p2, offer->entities) {
				if(p2->jid == player->jid) count++;
				//if(p2->player.parcialCompare(player->player)) count++;
			}
			if(count>1)
				throw "Players must not repeat";
		}
		if(not sanity) throw "Invalid message";
		if(not Util::hasKey(this->rules, offer->category))
			throw "Invalid category";
		match = auto_ptr<Match>((*this->rules.find(offer->category)).second->checkOffer(offer.release(),
				this->teams));
	} catch (const char* msg) {
		this->component.send(Stanza::createErrorStanza(stanza.release(), "cancel", "bad-request", msg));
		return;
	}
	int id = this->match_db.insertMatch(match.release());
	this->component.send(Stanza::createIQResult(stanza.release()));
	this->notifyMatchOffer(id);
	/*
	query* = this->protocol.answerMatchOffer(match_ref, id);
	stanza->children().push_back(query);
	stanza->subtype() = "set";
	foreach(team, match_ref.getTeams()) {
		foreach(player, *team) {
			stanza->to() = player->jid;
			this->root_node.sendIq(stanza->clone());
		}
	}
	*/
}

void MatchManager::notifyMatchOffer(int id) {
	const Match& match = this->match_db.getMatch(id);
	Stanza stanza("iq");
	stanza.subtype() = "set";
	stanza.children().push_back(this->protocol.notifyMatchOffer(match, id));
	foreach(team, match.getTeams()) {
		foreach(player, *team) {
			stanza.to() = player->jid;
			this->root_node.sendIq(stanza.clone());
		}
	}
}

void MatchManager::handleMatchAccept(Stanza* stanza) {
	try {
		cout << "incoming accept" << endl;
		XML::Tag& query = *stanza->children().front();
		int id = this->protocol.parseMatchAccept(query);
		if(this->match_db.acceptMatch(id, stanza->from()))
			this->notifyMatchResult(id, true);
		this->component.send(Stanza::createIQResult(stanza));
	} catch (const char* msg) {
		this->component.send(Stanza::createErrorStanza(stanza, "cancel", "bad-request", msg));
	}
}

void MatchManager::handleMatchDecline(Stanza* stanza) {
	try {
		XML::Tag& query = *stanza->children().front();
		int id = this->protocol.parseMatchDecline(query);
		if(not this->match_db.hasPlayer(id, stanza->from()))
			throw "Invalid id";
		this->component.send(Stanza::createIQResult(stanza));
		this->notifyMatchResult(id, false);
	} catch (const char* msg) {
		this->component.send(Stanza::createErrorStanza(stanza, "cancel", "bad-request", msg));
	}
}

void MatchManager::notifyMatchResult(int id, bool accepted) {
	Match* match = this->match_db.closeMatch(id);
	Stanza stanza("iq");
	stanza.subtype() = "set";
	stanza.children().push_back(this->protocol.notifyMatchResult(*match, id, accepted));
	foreach(team, match->getTeams()) {
		foreach(player, *team) {
			stanza.to() = player->jid;
			this->root_node.sendIq(stanza.clone());
		}
	}
	delete match;
}

void MatchManager::notifyUserStatus(XMPP::Jid jid, bool available) {
	if(not available) {
		set<int> matchs = this->match_db.getPlayerMatchs(jid);
		foreach(id, matchs) {
			this->notifyMatchResult(*id, false);
		}
	}
}
