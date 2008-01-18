#include <iostream>

#include <ctime>

#include "Core.hh"
#include "CoreInterface.hh"
#include "MatchManager.hh"

#include "Util/utils.hh"


Core* Core::_singleton = 0;

Core::Core(const XML::Tag& config_xml) :
    database_manager(config_xml.findChild("database")),
	match_manager(config_xml.findChild("match-manager"),
        boost::bind(&Core::handleError, this, _1)),
	game_manager(config_xml.findChild("game-manager"),
        boost::bind(&Core::handleError, this, _1)),
	rating_component(config_xml.findChild("rating-manager"),
        boost::bind(&Core::handleError, this, _1),
        this->database_manager)
{
}

void Core::connect() {
	this->dispatcher.start();
	this->match_manager.connect();
	this->game_manager.connect();
	this->rating_component.connect();
}

Core::~Core() {
	this->rating_component.close();
	this->game_manager.close();
	this->match_manager.close();
	this->dispatcher.stop();
}

void Core::init(const XML::Tag& config_xml) {
	Core::_singleton = new Core(config_xml);
}

void Core::destroy() {
	delete Core::_singleton;
}

void Core::handleError(const std::string& error) {
	std::cerr << "Core ERROR " << error << std::endl;
}

void Core::startGame(Game* game) {
	this->dispatcher.queue(boost::bind(&Core::_startGame, this, game));
}

void Core::endGame(int game_id, GameResult* result) {
	this->dispatcher.queue(boost::bind(&Core::_endGame, this, game_id, result));
}

void Core::adjournGame(int game_id, GameResult* result) {
	this->dispatcher.queue(boost::bind(&Core::_adjournGame, this, game_id, result));
}

void Core::cancelGame(int game_id) {
	this->dispatcher.queue(boost::bind(&Core::_cancelGame, this, game_id));
}

void finishGame(GameResult* result, DatabaseInterface& database) {
    RatingDatabase& rating_database = database.rating_database;
    GameDatabase& game_database = database.game_database;
    std::string category = result->category();
    PersistentGame game;

	std::map<Player, Rating> tmp;
	foreach(player, result->players()) {
		tmp.insert(std::make_pair(*player, rating_database.getRatingForUpdate(player->full(), category)));
        game.players.push_back(player->full());
	}
	result->updateRating(tmp);
	foreach(it, tmp) {
        rating_database.setRating(it->first.full(), category, it->second);
	}

    game.category = category;
    game.result = result->end_reason();
    game.time_stamp = time(NULL);

    XML::Tag* history = result->history();
    game.history = history->xml();
    delete history;

    game_database.insertGame(game);
	delete result;
}

void Core::_endGame(int game_id, GameResult* result) {
	this->game_ids.releaseID(game_id);
    this->database_manager.queueTransaction(boost::bind(finishGame, result, _1));
}

void Core::_adjournGame(int game_id, GameResult* result) {
	this->game_ids.releaseID(game_id);
	delete result;
}

void Core::_cancelGame(int game_id) {
	this->game_ids.releaseID(game_id);
}

void Core::_startGame(Game* game) {
	int game_id = this->game_ids.acquireID();
	this->game_manager.insertGame(game_id, game);
}
