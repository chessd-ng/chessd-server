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

#ifndef CORE_HH
#define CORE_HH

#include "Threads/Dispatcher.hh"

#include "XML/Xml.hh"

#include "MatchManager.hh"
#include "GameManager.hh"
#include "RatingComponent.hh"

#include "DatabaseManager.hh"

#include <string>
#include <set>

#include "Game.hh"
#include "Rating.hh"

/*! \brief This class is the implementation of the server core.
 *
 *  It is responsible to interconnect all of the elements of
 *  the server. */

typedef std::map<std::string, Rating> UserRatings;

typedef boost::function<void (UserRatings*)> RatingCallback;


class Core {
	public:


		~Core();

		/*! \brief Connects to the XMPP server.
		 *
		 * \param config holds all configurable information.
		 */
		void connect();

		static Core& singleton() { return *Core::_singleton; }

		static void init(const XML::Tag& config); 

		static void close();

	private:

		Core(const XML::Tag& config);

		void handleError(const std::string& error);

        DatabaseManager database_manager;

		GameManager game_manager;
		MatchManager match_manager;
		RatingComponent rating_component;


		static Core* _singleton;

		Threads::Dispatcher dispatcher;

		Util::IDSet game_ids;

		std::map<XMPP::Jid, std::map<std::string, Rating> > ratings;
};

#endif
