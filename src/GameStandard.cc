#include "GameStandard.hh"
#include <iostream>
#include "Util/utils.hh"
//TODO
//tempo
//testar XML

GameStandard::GameStandard(const StandardPlayerList& _players) :
	GameChess(_players),
	_category("Standard")
{
}
const std::string& GameStandard::category() const {
	return _category;
}
