#include <iostream>
#include <cmath>
#include "Util/utils.hh"
#include "GameStandard.hh"
#include "GlickoSystem.hh"
//TODO
//tempo
//testar XML

GameStandard::GameStandard(const StandardPlayerList& _players) :
	GameChess(_players),
	_category("standard")
{
}

const std::string& GameStandard::category() const {
	return _category;
}

GameResult* GameStandard::newGameResult(const std::string& endreason, const TeamResultList &l, const std::vector<State> &s) const {
	return new ChessStandardGameResult(endreason,l,s);
}

ChessStandardGameResult::ChessStandardGameResult(const std::string &endreason,const TeamResultList &l,const std::vector<State> &s) :
	ChessGameResult(endreason,l,"standard",s)
{
}

void ChessStandardGameResult::updateRating(std::map<Player, Rating> &ratings) const {
	std::vector<Player> playerlist;
	foreach(it,teamresultlist)
		playerlist.push_back(it->first[0]);

	int i=0,j=1;
	foreach(it,teamresultlist) {
		//TODO
		//Determine Step 1 of glicko System that it has to determine the 
		//new RD(Rating deviation) that is based on time of the last competition
		double w=0.0;
		if(it->second == WINNER)
			w=1.0;
		else if(it->second == DRAWER)
			w=0.5;
		double E=GlickoSystem::E(ratings[playerlist[i]],ratings[playerlist[j]]);

		double fs2=GlickoSystem::gRD(ratings[playerlist[i]]);

		double denominator=1.0/(GlickoSystem::square(ratings[playerlist[i]].volatility())) + GlickoSystem::square(GlickoSystem::q()) * GlickoSystem::square(fs2) * E * (1.0 - E);

		double GK = GlickoSystem::q()*fs2/denominator;

		double deltarating=round(GK * (w-E));

		double newvolatility=1.0/sqrt(denominator);

		ratings[playerlist[i]].rating()+=deltarating;

		ratings[playerlist[i]].volatility()=newvolatility;
		i++;
		j=(j+1)%2;
	}
}
