#include "Core.hh"
#include "CoreInterface.hh"
#include "MatchManager.hh"

#include <iostream>

Core* Core::_singleton = 0;

Core::Core(const XML::Tag& config_xml) :
	match_manager(config_xml.getChild("match-manager"), boost::bind(&Core::handleError, this, _1)),
	game_manager(config_xml.getChild("game-manager"), boost::bind(&Core::handleError, this, _1)) { }

void Core::connect() {
	this->dispatcher.start();
	this->match_manager.connect();
	this->game_manager.connect();
}

Core::~Core() {
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

//FIXME
void Core::_endGame(int game_id, GameResult* result) {
	this->game_ids.releaseID(game_id);
	std::map<Player, Rating> tmp;
	foreach(player, result->players()) {
		tmp.insert(std::make_pair(*player, this->ratings[std::make_pair(*player, result->category())]));
	}
	result->updateRating(tmp);
	foreach(player, result->players()) {
		this->ratings[make_pair(*player, result->category())] = tmp[*player];
	}
	delete result;
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

