#include "CoreInterface.hh"
#include "Core.hh"

CoreInterface::CoreInterface() : core(Core::singleton()) { }

CoreInterface::~CoreInterface() { }

// TODO

void CoreInterface::startGame(Game* game) {

}

void CoreInterface::endGame(int game_id, GameResult* result) {

}

void CoreInterface::adjournGame(int game_id, GameResult* result) {

}

void CoreInterface::cancelGame(int game_id) {

}

