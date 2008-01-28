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

#ifndef GAMECHESS_HH
#define GAMECHESS_HH

#include "Game.hh"
//#include "Chesslib.hh"
#include "Util/Timer.hh"
#include "libchess/Chess.hh"
#include "XMPP/Jid.hh"
#include <vector>


//TODO ver negocios do tempo!
class GameChess : public Game {
	public:
		GameChess(const StandardPlayerList& _players, const std::string &category);

		virtual ~GameChess() {};

		virtual XML::Tag* state() const;

		virtual const std::string& category() const;
		
		/*! \brief function to return the title of the game
		 * \return returns the full jids of the player separated by an "x" */
		virtual const std::string& title() const;

		/*! \brief The player has resigned */
		virtual void resign(const Player& player);

		/*! \brief The player has called a flag */
		virtual void call_flag(const Player& player); //TODO

		/*! \brief The players agreed on a draw */
		virtual void draw();

		virtual void adjourn(); //TODO

		/*! \brief Has the game ended?
		 * \return Returns the game result if the game is over, NULL otherwise.
		 */
		virtual GameResult* done() const;

		virtual void move(const Player& player, const std::string& movement);

		virtual const TeamList& teams() const;

		static XML::Tag* generateStateTag(const State &est) ;
		
		virtual GameResult* newGameResult(const std::string& endreason, const TeamResultList &l, const std::vector<State> &s) const = 0;
	private:
		Chess chess;
		TeamList _teams;
		std::string _title;
		std::string _category;

		//unify the color name
		Chess::Color _resign;
		bool _draw;

		Util::Time whitetime;
		//map de player -> cor
		std::map<Player,Chess::Color> colormap;
		StandardPlayerList _players;
};

class ChessGameResult : public GameResult {
	public:
		ChessGameResult(const std::string& endreason,const TeamResultList &l,const std::string& _category,const std::vector<State> &s);

		virtual ~ChessGameResult(){};

		virtual const std::string& category() const;

		virtual const std::string& end_reason() const;

		virtual const PlayerList& players() const;

		virtual const TeamResultList& results() const;

		/*! \brief generates a History tag*/
		virtual XML::Tag* history() const;

		virtual void updateRating(std::map<Player, Rating>& ratings) const = 0;
	protected:
		TeamResultList teamresultlist;
	private:
		std::string _end_reason;
		PlayerList playerlist;
		std::string _category;
		std::vector<State> states;
};

#endif
