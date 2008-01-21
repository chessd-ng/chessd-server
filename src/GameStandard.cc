#include <iostream>
#include <cmath>
#include "Util/utils.hh"
#include "GameStandard.hh"
#include "GlickoSystem.hh"
//TODO
//tempo
//testar XML

GameStandard::GameStandard(const StandardPlayerList& _players) :
	GameChess(_players,"standard")
{
}

GameResult* GameStandard::newGameResult(const std::string& endreason, const TeamResultList &l, const std::vector<State> &s) const {
	return new ChessStandardGameResult(endreason,l,s);
}

ChessStandardGameResult::ChessStandardGameResult(const std::string &endreason,const TeamResultList &l,const std::vector<State> &s) :
	ChessGameResult(endreason,l,"standard",s)
{
}

void ChessStandardGameResult::updateRating(std::map<Player, Rating> &ratings) const {
	foreach(it,ratings) {
		if(it->second.countGames() == 0) {
			it->second.rating()=1500;
			it->second.volatility()=350.0;
		}
	}
	std::vector<Player> playerlist;
	std::vector<Rating> playerrating;
	foreach(it,teamresultlist) {
		playerlist.push_back(it->first[0]);
		playerrating.push_back(ratings[it->first[0]]);
	}

	int i=0,j=1;
	foreach(it,teamresultlist) {
		//TODO
		//Do Step 1 of glicko System that it determines the new RD(Rating deviation)
		//that is based on the difference between now and the date since the last competition
		//in seconds
		//Just need to receive the last time the player has played
		double w;
		if(it->second == WINNER) {
			playerrating[i].wins()++;
			w=1.0;
		}
		else if(it->second == DRAWER) {
			playerrating[i].draws()++;
			w=0.5;
		}
		else { //it->second = LOSER
			playerrating[i].losses()++;
			w=0.0;
		}
		double E=GlickoSystem::E(ratings[playerlist[i]],ratings[playerlist[j]]);

		double fs2=GlickoSystem::gRD(ratings[playerlist[j]]);
//		economical way
		double denominator=1.0/(GlickoSystem::square(ratings[playerlist[i]].volatility())) + GlickoSystem::square(GlickoSystem::q()) * GlickoSystem::square(fs2) * E * (1.0 - E);

//		Pratical way
//		double denominator=1.0/(GlickoSystem::square(ratings[playerlist[i]].volatility())) + 1.0/GlickoSystem::dsquare(ratings[playerlist[i]],ratings[playerlist[j]]);

		playerrating[i].rating()+=GlickoSystem::round(GlickoSystem::q()*fs2*(w-E)/denominator);

		playerrating[i].volatility()=1.0/sqrt(denominator);
		i++;
		j=(j+1)%2;
	}
	for(i=0;i<(int)playerrating.size();i++) {
		ratings[playerlist[i]]=playerrating[i];
	}
}