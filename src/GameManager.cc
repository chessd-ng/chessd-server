#include "GameManager.hh"
#include "GameProtocol.hh"
#include "Util/utils.hh"

using namespace XMPP;
using namespace std;

GameManager::GameManager(const XML::Tag& config, const XMPP::ErrorHandler& handle_error) :
	ComponentBase(config, "Game Manager"),
	node_name(config.getAttribute("node_name")),
	handle_error(handle_error)
{
	//FIXME
	GameProtocol::init("protocol");

	/* Set features */
	this->root_node.disco().features().insert("http://c3sl.ufpr.br/chessd#game");

	/* Set game iqs */
	this->root_node.setIqHandler(boost::bind(&GameManager::handleGame, this, _1),
			"http://c3sl.ufpr.br/chessd#game");

}

GameManager::~GameManager() {
}

void GameManager::onClose() {

}

void GameManager::onError(const string& msg) {
    this->handle_error(msg);
}

void GameManager::handleGame(Stanza* stanza) {
	XML::Tag& query = *stanza->children().tags().begin();
	try {
		string query_name = GameProtocol::parseQuery(query);
	} catch (const char* msg) {
		this->sendStanza(Stanza::createErrorStanza(stanza, "cancel", "bad-request", msg));
	}
	// nothing to do here yet
	delete stanza;
}

void GameManager::insertGame(int game_id, Game* game) {
	this->dispatcher.queue(boost::bind(&GameManager::_insertGame, this, game_id, game));
}

void GameManager::_insertGame(int game_id, Game* game) {
	int room_id = this->room_ids.acquireID();
	Jid room_jid = Jid("game_" + Util::to_string(room_id), this->node_name);
	/* Create the game room */
	GameRoom* game_room = new GameRoom(game_id, game, room_jid,
			GameRoomHandlers(boost::bind(&ComponentBase::sendStanza, this, _1),
				boost::bind(&GameManager::closeGameRoom, this, room_id)));
	/* Register the new jabber node */
	this->root_node.setNodeHandler(room_jid.node(),
			boost::bind(&GameRoom::handleStanza, game_room, _1));
	/* Add the new jabber node to the disco */
	this->root_node.disco().items().insert(new XMPP::DiscoItem(room_jid.node(),
				room_jid));
	game_rooms.insert(room_id, game_room);
}

void GameManager::closeGameRoom(int room_id) {
	this->dispatcher.queue(boost::bind(&GameManager::_closeGameRoom, this, room_id));
}

void GameManager::_closeGameRoom(int room_id) {
	this->room_ids.releaseID(room_id);
	Jid room_jid = Jid("game_" + Util::to_string(room_id), this->node_name);
	this->root_node.removeNodeHandler(room_jid.node());
	this->root_node.disco().items().erase(room_jid);
	this->game_rooms.erase(room_id);
}

void GameManager::handleError(const std::string& error) {
	this->close();
	this->handle_error(error);
}
