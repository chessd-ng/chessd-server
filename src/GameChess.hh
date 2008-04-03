/*
 *   Copyright (c) 2007-2008 C3SL.
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
#include "Util/Timer.hh"
#include "libchess/Chess.hh"
#include "XMPP/Jid.hh"
#include <vector>

enum end_reason{
	NOREASON=0,RESIGNED=1,CHECKMATE=2,DRAWAGREED=3,TIMEOVER=4,DRAWREPETITION=5,DRAWIMPOSSIBILITYOFCHECKMATE=6,DRAWFIFTYMOVE=7,DRAWSTALEMATE=8
};


class GameChess : public Game {
	public:
		GameChess(const StandardPlayerList& _players, const std::string &category);

		/*! \brief Constructor for adjourned games
		 * \description it assumes that adjourned games are not over
		*/
		GameChess(XML::Tag* adjourned_game);

		virtual ~GameChess() {};

		virtual XML::Tag* state(const Util::Time& current_time) const;

		virtual XML::Tag* history() const;

        virtual const std::vector<XMPP::Jid>& players() const;

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

		virtual AdjournedGame* adjourn(const Util::Time& current_time);

		/*! \brief Has the game ended?
		 * \return true if the game is over of false if it is not over
		 * \description it checks if the time is over and the value of this->_done
		 * since this function can be called several times, this->done was created
		 * for optimizations, an it's set on functions move,resign and draw()
		 */
		virtual bool done(const Util::Time& current_time) ;

		virtual GameResult* result() const ;

		virtual XML::Tag* move(const Player& player, const std::string& movement, const Util::Time& time_stamp);

	protected:
		/*! \brief returns the end reason if the game has ended*/
		virtual std::string doneEndReason() const;

		/*! \brief returns the team result list if the game has ended*/
		virtual PlayerResultList donePlayerResultList() const;

		int realDone();

		//! \brief generates state tag as specified in chessd protocol
		XML::Tag* generateStateTag(const ChessState &est,const Util::Time& current_time) const ;
		
		//! \brief generates history tag as specified in chessd protocol
		XML::Tag* generateHistoryTag(Util::Time time_passed=Util::Time()) const;

		//Chess class from libchess
		Chess chess;

		//stores the time of the last move
		//at the beginning of the game or at restart time_of_last_time is 0
		Util::Time time_of_last_move;

	private:
		//\brief set initial variables, it is just called in the constructor
		void setInitialVariables();

		//\brief check if the time of current player is over,
		//if yes, then sed this->_done and this->time_over
		bool checkTimeOver(const Util::Time& current_time);

		/*
		 * tells if the time of one player is over
		 * -1 for no time over
		 *  0 white's time is over
		 *  1 black's time is over
		*/
		int time_over;

		/*
		 * store the initial time of the game
		*/
		int initial_time;

		/*
		 * The time is only counted after 2 moves after the game start or restart
		 * stores the number of turns since the restart or start of the game
		*/
		int turns_restart;

		/*
		 * for optimizations reasons, the reason why the game is over is set on move
		 * and when done is called, it's only needed to see the value of this variable
		 * and if the time is over
		*/
		end_reason _done;

		/*
		 * if a player resigned, stores his color
		 * Chess::UNDEFINED for none resigned
		 * Chess::WHITE for white resigned
		 * Chess::BLACK for black resigned
		*/
		Chess::Color _resign;

		//stores the title of the game "jid white x jid black"
		std::string _title;

		//stores wich catecory the game is
		std::string _category;

		/*
		 * libchess is based in color, and this API is based on jid, so
		 * this map make the correlations between jid and color
		*/
		std::map<Player,Chess::Color> colormap;

		/*
		 * the best structure to deal with chessplayers is the StandardPlayer
		 * and this API is based on Player(JID), so this correlation is done here
		*/
		std::map<Player,StandardPlayer*> standard_player_map;

		//a list of StandardPlayers
		StandardPlayerList _players;

		//stores the history of the game
		std::string history_moves;

		//a list of Players
        PlayerList _simple_players;
};

class ChessGameResult : public GameResult {
	public:
		ChessGameResult(const std::string& endreason,const PlayerResultList &l,const std::string& _category,XML::Tag *hist);

		virtual ~ChessGameResult();

		virtual const std::string& category() const;

		virtual const std::string& end_reason() const;

		virtual const PlayerResultList& players() const;

		/*! \brief generates a History tag*/
		virtual XML::Tag* history() const;

		virtual void updateRating(std::map<Player, Rating>& ratings) const;

	protected:
		PlayerResultList player_result_list;

	private:
		std::string _end_reason;

		std::string _category;

		XML::Tag *_history;
};

class ChessAdjournedGame : public AdjournedGame {
	public:
		ChessAdjournedGame(XML::Tag* history, const PlayerList& list, const std::string& category);

		~ChessAdjournedGame();

		virtual XML::Tag* history() const;

		virtual const PlayerList& players() const;

        virtual const std::string& category() const;
	private:
		XML::Tag* _history;

		PlayerList _players;

        std::string _category;
};

#endif
