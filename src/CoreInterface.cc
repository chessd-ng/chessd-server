#include "CoreInterface.hh"
#include "Core.hh"

CoreInterface::CoreInterface() { }

CoreInterface::~CoreInterface() { }

void CoreInterface::startGame(Game* game) {
    Core::singleton().startGame(game);
}

void CoreInterface::endGame(int game_id, GameResult* result) {
    Core::singleton().endGame(game_id, result);
}

void CoreInterface::adjournGame(int game_id, GameResult* result) {
    Core::singleton().adjournGame(game_id, result);
}

void CoreInterface::cancelGame(int game_id) {
    Core::singleton().cancelGame(game_id);
}

void CoreInterface::fetchUserRatings(const XMPP::Jid& user, const RatingCallback& callback) {
    Core::singleton().fetchUserRatings(user, callback);
}

