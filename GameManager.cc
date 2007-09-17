#include "GameManager.hh"
#include "GameProtocol.hh"

GameManager::GameManager(const XML::Tag& config) :
	running(false),
	component(config.getAttribute("node_name"),
			this->dispatcher.createTunnel(boost:bind(&Node::handleStanza, &this->root_node))),
	node_name(config.getAttribute("node_name")),
	listener(this->component),
	root_node(boost::bind(&Component::send, &this->component, _1), "Game Manager", "service", "game"),
	server_address(config.getAttribute("server_address")),
	server_port(Util::str2int(config.getAttribute("server_port"))),
	server_password(config.getAttribute("server_password")),
	insert_game_tunnel(this->dispatcher.createTunnel(
				boost::bind(&GameManager::_insertGame, this, _1, _2)))
{
	/* Set features */
	this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#game");

	/* Set match iqs */
	this->root_node.setIqHandler(boost::bind(&MatchManager::handleGame, this, _1),
			"http://c3sl.ufpr.br/chessd#game");
}

GameManager::~GameManager() {
	if(this->running)
		this->close();
}

bool GameManager::connect() {
	if(not this->component.connect(this->server_address, this->server_port, this->server_password)) {
		return false; // TODO throw an exception
	} else {
		this->running = true;
		this->dispatcher.start();
		this->listener.start();
	}
}

void GameManager::close() {
	this->listener.stop();
	this->dispatcher.stop();
	this->component.close();
	this->running = false;
}

void GameManager::handleGame(Stanza* stanza) {
	Tag& query = *stanza->children().front();
	try {
		std::string = GameProtocol::parseGameQuery(query);
	} catch (const char* msg) {
		this->component.send(Stanza::createErrorStanza(stanza, "cancel", "bad-request", msg));
	}
	// nothing to do here yet
	delete stanza;
}

void GameManager::insertGame(Game* game, int game_id) {
	this->insert_game_tunnel(game, game_id);
}

void GameManager::_insertGame(Game* game, int game_id) {
	GameRoom* game_room = new GameRoom(game, game_id,
			boost::bind(&GameManager::handleGameResult, this, _1, game_id));
	this->component.setStanzaHandler(this->dispatcher.createTunnel(boost::bind(
					&GameRoom::handleStanza, game_room, _1)), "game_" + Utils::int2str(game_id));
	this->root_node.disco().items().insert(XMPP::DiscoItem("game_" + Utils::int2str(game_id),
				XMPP::Jid(this->node_name + "@game_" + Utils::int2str(game_id))));
	game_rooms.insert(game_id, game_room);
}

GameRoom::GameRoom(Game* game, int game_id): game(game), game_id(game_id) { }

GameRoom::~GameRoom() {
}
