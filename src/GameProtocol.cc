#include "GameProtocol.hh"

XML::Description GameProtocol::query_desc;
XML::Description GameProtocol::move_desc;

void GameProtocol::init(const std::string& path) {
	if(not GameProtocol::query_desc.loadFromFile(path + "/game_query.desc"))
		throw "Could not load game_query.desc";
	if(not GameProtocol::move_desc.loadFromFile(path + "/game_move.desc"))
		throw "Could not load game_move.desc";
}

std::string GameProtocol::parseQuery(XML::Tag& query) {
	if(not GameProtocol::query_desc.validateXML(query))
		throw "Invalid syntax";
	return query.getAttribute("action");
}

std::string GameProtocol::parseMove(XML::Tag& query) {
	if(not GameProtocol::move_desc.validateXML(query))
		throw "Invalid syntax";
	return query.findChild("move").getAttribute("long");
}
