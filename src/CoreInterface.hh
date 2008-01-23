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

#ifndef CORE_INTERFACE_HH
#define CORE_INTERFACE_HH

#include <boost/function.hpp>
#include "XMPP/Jid.hh"
#include "Game.hh"

typedef std::map<std::string, Rating> UserRatings;

typedef boost::function<void ()> ShutdownHandler;
typedef boost::function<void (UserRatings*)> RatingCallback;

class Core;

class CoreInterface {
	public:
		/*! \brief destroys the interface */
		~CoreInterface();

		CoreInterface();



		/*! \brief Request the Core to start a game.
		 *
		 * The Core is responsible for spawning the game,
		 * Returns true in case of success, false otherwise
		 */
		void startGame(Game* game);

		void endGame(int game_id, GameResult* result);
		void adjournGame(int game_id, GameResult* result);
		void cancelGame(int game_id);

	private:

		void shutdown();

};

#endif
