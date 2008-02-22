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

#include <iostream>
#include <cmath>
#include "Util/utils.hh"
#include "GameStandard.hh"
//TODO
//tempo
//testar XML

GameStandard::GameStandard(const StandardPlayerList& _players) :
	GameChess(_players,"standard")
{
}

GameResult* GameStandard::result() const {
	return new ChessStandardGameResult(this->doneEndReason(),this->doneTeamResultList(),this->generateHistoryTag());
}

ChessStandardGameResult::ChessStandardGameResult(const std::string &endreason,const TeamResultList &l,XML::Tag* _hist) :
	ChessGameResult(endreason,l,"standard",_hist)
{
}
