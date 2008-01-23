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

#ifndef GAMEPROTOOL_HH
#define GAMEPROTOOL_HH

#include "XML/Xml.hh"

class GameProtocol {
	public:
		static std::string parseQuery(XML::Tag& query);

		static std::string parseMove(XML::Tag& query);

		static void init(const std::string& path);

		//static std::string parseGameRoomQuery(XML::Tag& query);

	private:
		GameProtocol() { }

		~GameProtocol() { }

		static XML::Description query_desc;
		static XML::Description move_desc;

};

#endif
