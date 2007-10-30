#include "CoreInterface.hh"
#include "Core.hh"

CoreInterface::CoreInterface() : core(Core::singleton()) { }

CoreInterface::~CoreInterface() { }

// TODO

void CoreInterface::startGame(Game* game) {
	this->core.startGame(game);

}

void CoreInterface::endGame(int game_id, GameResult* result) {
	this->core.endGame(game_id, result);
}

void CoreInterface::adjournGame(int game_id, GameResult* result) {
	this->core.adjournGame(game_id, result);
}

void CoreInterface::cancelGame(int game_id) {
	this->core.cancelGame(game_id);
}

