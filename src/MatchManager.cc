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

MatchManager::MatchManager(const XML::Tag& config, const XMPP::ErrorHandler& handleError) :
	running(false),
	component(
			ComponentWrapperHandlers(boost::bind(&MatchManager::_handleError, this, _1)),
			config.getAttribute("node_name"), 
			boost::bind(&MatchManager::handleStanza, this, _1)),
	root_node(boost::bind(&ComponentWrapper::sendStanza, &this->component, _1),
			XMPP::Jid(config.getAttribute("node_name")),
			"Match Manager", "service", "game"),
	roster(boost::bind(&RootNode::sendStanza, &this->root_node, _1),
			boost::bind(&MatchManager::notifyUserStatus, this, _1, _2)),
	server_address(config.getAttribute("server_address")),
	server_port(Util::str2int(config.getAttribute("server_port"))),
	server_password(config.getAttribute("server_password")),
	handleError(handleError)
{

	this->dispatcher.start();

	/* Set the root_node to route presence stanzas to the roster */
	this->root_node.setPresenceHandler(boost::bind(&Roster::handlePresence, &this->roster, _1));

	/* Set features */
	this->root_node.disco().features().insert("presence");
	this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#match");

	/* Set match iqs */
	this->root_node.setIqHandler(boost::bind(&MatchManager::handleMatch, this, _1),
			"http://c3sl.ufpr.br/chessd#match");

	/* FIXME */
	this->insertMatchRule(new MatchRuleStandard);

	MatchProtocol::init("protocol");
}

void MatchManager::insertMatchRule(MatchRule* rule) {
	std::string category = rule->getCategory();
	this->rules.insert(category, rule);
}

MatchManager::~MatchManager() {
	this->close();
	this->dispatcher.stop();
}

void MatchManager::connect() {
	this->running = true;
	this->component.connect(this->server_address, this->server_port, this->server_password);
}

void MatchManager::close() {
	if(this->running) {
		this->running = false;
		this->component.close();
	}
}

void MatchManager::handleMatch(Stanza* stanza) {
	Tag& query = stanza->children().front();
	string action;
	try {
		action = MatchProtocol::parseMatch(query);
	} catch (const char* msg) {
		this->component.sendStanza(Stanza::createErrorStanza(stanza, "cancel", "bad-request", msg));
		return;
	}
	if(action == "offer") {
		this->handleMatchOffer(stanza);
	} else if(action == "accept") {
		this->handleMatchAccept(stanza);
	} else if(action == "decline") {
		this->handleMatchDecline(stanza);
	} else {
		this->component.sendStanza(Stanza::createErrorStanza(stanza, "cancel", "bad-request"));
	}
}

void MatchManager::handleMatchOffer(Stanza* _stanza) {
	//auto_ptr<MatchOffer> offer;
	auto_ptr<Match> match;
	auto_ptr<Stanza> stanza(_stanza);
	Jid requester = stanza->from();
	try {
		/* parse message */
		XML::Tag& offer = *stanza->children().begin();
		//offer = auto_ptr<MatchOffer>(MatchProtocol::parseMatchOffer(*stanza->children().begin()));

		/* check players availability and whether
		 * to sender is one of the entities */
		/*bool sanity = false;
		foreach(player, offer->entities) {
			if(not this->roster.isUserAvailable(player->jid))
				throw "User is unavailable";
			if(player->jid == requester) sanity = true;
			int count = 0;
			foreach(p2, offer->entities) {
				if(p2->jid == player->jid) count++;
			}
			if(count>1)
				throw "Players must not repeat";
		}
		if(not sanity) throw "Invalid message";*/
		if(not offer.hasAttribute("category"))
			throw "Invalid message";
		const std::string& category = offer.getAttribute("category");
		if(not Util::has_key(this->rules, category))
			throw "Invalid category";
		match = auto_ptr<Match>((*this->rules.find(category)).second->checkOffer(offer,
				this->teams));
	} catch (const char* msg) {
		this->component.sendStanza(Stanza::createErrorStanza(stanza.release(), "cancel", "bad-request", msg));
		return;
	}
	int id = this->match_db.insertMatch(match.release());
	this->component.sendStanza(Stanza::createIQResult(stanza.release()));
	this->notifyMatchOffer(id);
	/*
	query* = MatchProtocol::answerMatchOffer(match_ref, id);
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
	stanza.children().push_back(match.notification());
	foreach(player, match.players()) {
		stanza.to() = *player;
		this->root_node.sendIq(stanza.clone());
	}
}

void MatchManager::handleMatchAccept(Stanza* stanza) {
	try {
		cout << "incoming accept" << endl;
		XML::Tag& query = stanza->children().front();
		int id = MatchProtocol::parseMatchAccept(query);
		if(this->match_db.acceptMatch(id, stanza->from()))
			this->notifyMatchResult(id, true);
		this->component.sendStanza(Stanza::createIQResult(stanza));
	} catch (const char* msg) {
		this->component.sendStanza(Stanza::createErrorStanza(stanza, "cancel", "bad-request", msg));
	}
}

void MatchManager::handleMatchDecline(Stanza* stanza) {
	try {
		XML::Tag& query = stanza->children().front();
		int id = MatchProtocol::parseMatchDecline(query);
		if(not this->match_db.hasPlayer(id, stanza->from()))
			throw "Invalid id";
		this->component.sendStanza(Stanza::createIQResult(stanza));
		this->notifyMatchResult(id, false);
	} catch (const char* msg) {
		this->component.sendStanza(Stanza::createErrorStanza(stanza, "cancel", "bad-request", msg));
	}
}

void MatchManager::notifyMatchResult(int id, bool accepted) {
	Match* match = this->match_db.closeMatch(id);
	Stanza stanza("iq");
	stanza.subtype() = "set";
	stanza.children().push_back(MatchProtocol::notifyMatchResult(*match, id, accepted));
	foreach(player, match->players()) {
		stanza.to() = *player;
		this->root_node.sendIq(stanza.clone());
	}
	if(accepted)
		this->core_interface.startGame(match->createGame());
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

void MatchManager::_handleError(const std::string& error) {
	this->close();
	this->handleError(error);
}

void MatchManager::handleStanza(XMPP::Stanza* stanza) {
	this->dispatcher.queue(boost::bind(&XMPP::RootNode::handleStanza, &this->root_node, stanza));
}
