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

void MatchManager::handleMatch(Stanza* _stanza) {
    std::auto_ptr<Stanza> stanza(_stanza);
    std::auto_ptr<Query> query;
	try {
        query = std::auto_ptr<Query>(new Query(move(*stanza)));
	} catch (const char* msg) {
		this->component.sendStanza(
                Stanza::createErrorStanza(
                        stanza.release(),
                        "cancel",
                        "bad-request",
                        msg));
		return;
	}
	if(query->action() == "offer") {
		this->handleMatchOffer(query.release());
	} else if(query->action() == "accept") {
		this->handleMatchAccept(query.release());
	} else if(query->action() == "decline") {
		this->handleMatchDecline(query.release());
	} else {
		this->component.sendStanza(
                Stanza::createErrorStanza(
                        Query::createStanza(move(*query)),
                        "cancel",
                        "bad-request"));
	}
}

void MatchManager::handleMatchOffer(Query* _query) {
	//auto_ptr<MatchOffer> offer;
	auto_ptr<Match> match;
	auto_ptr<Query> query(_query);
	Jid requester = query->from();
	try {
		/* parse message */
		XML::Tag& offer = query->children().tags().front();
		if(not offer.hasAttribute("category"))
			throw "Invalid message";
		const std::string& category = offer.getAttribute("category");
		if(not Util::has_key(this->rules, category))
			throw "Invalid category";
		match = auto_ptr<Match>((*this->rules.find(category)).second->checkOffer(offer,
				this->teams));
	} catch (const char* msg) {
		this->component.sendStanza(
                Stanza::createErrorStanza(
                        Query::createStanza(move(*query)),
                        "cancel",
                        "bad-request",
                        msg));
		return;
	}
	int id = this->match_db.insertMatch(match.release());
    this->match_db.acceptMatch(id, requester);
	this->component.sendStanza(Stanza::createIQResult(Query::createStanza(move(*query))));
	this->notifyMatchOffer(id, requester);
}

void MatchManager::notifyMatchOffer(int id, const Jid& requester) {
	const Match& match = this->match_db.getMatch(id);
	Stanza stanza("iq");
	stanza.subtype() = "set";
	Tag* tag = match.notification();
    tag->setAttribute("id", Util::int2str(id));
    stanza.children().push_back(tag);
	foreach(player, match.players()) {
        if(*player != requester) {
            stanza.to() = *player;
            this->root_node.sendIq(stanza.clone());
        }
	}
}

void MatchManager::handleMatchAccept(Query* _query) {
    std::auto_ptr<Query> query(_query);
	try {
		cout << "incoming accept" << endl;
        if(query->children().tags().begin() ==  query->children().tags().end())
            throw "Invalid message";
		XML::Tag& match = query->children().tags().front();
        if(match.name() != "match" or not match.hasAttribute("id"))
            throw "Invalid message";
		int id = Util::str2int(match.getAttribute("id"));
        if(this->match_db.acceptMatch(id, query->from())) {
            this->closeMatch(id, true);
        }
		this->component.sendStanza(
                Stanza::createIQResult(
                        Query::createStanza(move(*query))));
	} catch (const char* msg) {
		this->component.sendStanza(
                Stanza::createErrorStanza(
                        Query::createStanza(move(*query)),
                        "cancel",
                        "bad-request",
                        msg));
	}
}

void MatchManager::handleMatchDecline(Query* _query) {
    std::auto_ptr<Query> query(_query);
	try {
		XML::Tag& match = query->children().tags().front();
        if(match.name() != "match" or not match.hasAttribute("id"))
            throw "Invalid message";
		int id = Util::str2int(match.getAttribute("id"));
		if(not this->match_db.hasPlayer(id, query->from()))
			throw "Invalid id";
		this->component.sendStanza(
                Stanza::createIQResult(
                        Query::createStanza(move(*query))));
        this->closeMatch(id, false);
	} catch (const char* msg) {
		this->component.sendStanza(
                Stanza::createErrorStanza(
                        Query::createStanza(move(*query)),
                        "cancel",
                        "bad-request",
                        msg));
	}
}

void MatchManager::closeMatch(int id, bool accepted) {
    Match* match = this->match_db.closeMatch(id);
    if(accepted) {
        this->core_interface.startGame(match->createGame());
    }
    this->notifyMatchResult(match, id, accepted);
}

void MatchManager::notifyMatchResult(Match* match, int id, bool accepted) {
	Stanza stanza("iq");
	stanza.subtype() = "set";
	stanza.children().push_back(MatchProtocol::notifyMatchResult(*match, id, accepted));
	foreach(player, match->players()) {
		stanza.to() = *player;
		this->root_node.sendIq(stanza.clone());
	}
	delete match;
}

void MatchManager::notifyUserStatus(XMPP::Jid jid, bool available) {
	if(not available) {
		set<int> matchs = this->match_db.getPlayerMatchs(jid);
		foreach(id, matchs) {
            this->closeMatch(*id, false);
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
