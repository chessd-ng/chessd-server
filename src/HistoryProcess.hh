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

#ifndef HISTORYPROCESS_HH
#define HISTORYPROCESS_HH

#include "XML/Tag.hh"
#include "libchess/Chess.hh"
#include <vector>

class HistoryProcess {
	public:
		HistoryProcess() {};

		virtual XML::Tag* generate(const XML::Tag& history)=0;
	private:
};

class ChessHistoryProcess : public HistoryProcess {
	public:
		ChessHistoryProcess() {};

		virtual XML::Tag* generate(const XML::Tag& history);

	private:
		static XML::Tag* generateStateTag(const ChessState& state, const std::vector<XML::Tag*>& players, const std::vector<int>& last_time, const std::string& category);
};

#endif
