/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

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
